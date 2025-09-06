#include <chrono>
#include <string>
#include <format>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>

#include <lua.hpp>

#include "yum.h"

static int writer(lua_State *, const void *p, size_t sz, void *ud) {
  std::ostream* out = static_cast<std::ostream*>(ud);
  out->write(reinterpret_cast<const char*>(p), sz);
  return out->good() ? 0 : 1;
}

class Resource {
private:
  Yumuid UID_Base;
  std::filesystem::path homedir;
  std::filesystem::path bindir;

  enum ResourceKind { WORLD, BIN, ERROR };
  enum ErrorCategory : int16_t { KILLEDUID, INVALIDUID, BADRESTYPE, LUA_ERROR, SYSTEM_ERROR };

  std::unordered_map<Yumuid, ResourceKind> alive;
  std::unordered_set<Yumuid>               deadResources;
  std::unordered_map<Yumuid, lua_State*>   states;
  std::unordered_map<Yumuid, std::string>  errors;
  std::unordered_map<Yumuid, std::string>  binaries;

  std::string kind2str(ResourceKind kind) {
    switch (kind) {
      case WORLD: return "world"; break;
      case BIN:   return "bin"; break;
      case ERROR: return "error"; break;
      default:    return "unknown";  break;
    }
  }

  YumErr makeError(const std::string &what, ErrorCategory e) {
    Yumuid uid = nuid();
    errors[uid] = what;
    return YumErr {
      .code = e,
      .uid = uid
    };
  }

  YumErr get(Yumuid uid, ResourceKind kind) {
    if (!alive.contains(uid)) {
      if (deadResources.contains(uid)) {
        return makeError(std::format("{}: killed", uid), KILLEDUID);
      } else return makeError(std::format("{}: unknown UID", uid), INVALIDUID);
    } else if (alive[uid] != kind) {
      return makeError(std::format("{}: invalid kind ({} (asked one): {})", uid, kind2str(kind), kind2str(alive[uid])), BADRESTYPE);
    }

    return YumErr {
      .code = 0, 
      .uid = uid
    };
  }

public:
  Yumuid nuid() { 
    return ++UID_Base;
  }

  std::string getError(Yumuid uid) {
    if (errors.contains(uid)) return errors[uid];

    return std::format("{}: not an error!", uid);
  }

  Yumuid createWorld() {
    Yumuid id = nuid();
    alive[id] = WORLD;
    states[id] = luaL_newstate();
    return id;
  }

  void destroyWorld(Yumuid id) {
    auto it = states.find(id);
    if (it != states.end()) {
      lua_close(it->second);
      states.erase(it);
    }
    alive.erase(id);
    deadResources.insert(id);
  }

  std::pair<YumErr, lua_State*> getLuaState(Yumuid uid) {
    YumErr err = get(uid, WORLD);
    if (err.code != 0) return {err, nullptr};
    return {err, states[uid]};
  }

  YumErr compile(const std::string& src, bool isAFile, bool stdLib) {
    lua_State* L = luaL_newstate();
    if (!L) return makeError("Failed to create Lua state", LUA_ERROR);

    int status = isAFile ? luaL_loadfile(L, src.c_str()) : luaL_loadstring(L, src.c_str());
    if (status != LUA_OK) {
      std::string err = lua_tostring(L, -1);
      lua_close(L);
      return makeError(std::format("Compile error in {}: {}", src, err), LUA_ERROR);
    }

    if (stdLib) luaL_openlibs(L);

    Yumuid id = nuid();
    alive[id] = BIN;

    std::filesystem::path outFile = bindir / std::format("yum_{}.luac", id);

    std::ofstream out(outFile, std::ios::binary);
    if (!out.is_open()) {
      lua_close(L);
      return makeError("Failed to open output file", SYSTEM_ERROR);
    }

    if (lua_dump(L, writer, &out, 0) != 0) {
      lua_close(L);
      return makeError("Failed to dump bytecode", LUA_ERROR);
    }

    out.close();
    lua_close(L);

    binaries[id] = outFile.string();

    return {0, id};
  }

  YumErr run(Yumuid world, Yumuid file) {
    YumErr errWorld = get(world, WORLD);
    YumErr errFile = get(file, BIN);
    if (errWorld.code != 0) return errWorld;
    if (errFile.code != 0) return errFile;

    int status = luaL_loadfile(states[world], binaries[file].c_str());
    if (status != LUA_OK) {
      std::string err = lua_tostring(states[world], -1);
      return makeError(std::format("Load error: {}", err), LUA_ERROR);
    }

    luaL_openlibs(states[world]);

    status = lua_pcall(states[world], 0, LUA_MULTRET, 0);
    if (status != LUA_OK) {
      std::string err = lua_tostring(states[world], -1);
      return makeError(std::format("Runtime error: {}", err), LUA_ERROR);
    }

    return {0, file};
  }


  Resource(Yumuid b, const std::filesystem::path &home) 
    : UID_Base(b), homedir(home), bindir(home / "bin") {
    if (!std::filesystem::exists(home)) {
      throw std::runtime_error("no such path: " + home.string());
    }    

    if (!std::filesystem::exists(bindir)) {
      std::filesystem::create_directories(bindir);
    }
  }

  ~Resource() {
    if (alive.size() != 0) std::cerr << "err: Yum: " << alive.size() << " resources are not killed -- memory leak...\n";
  }
};

static Resource *g_Resource;
static const constexpr YumErr YUM_NOT_READY = YumErr {
  .code = 0x1000,
  .uid  = YUM_UID_NULL
};

YumErr Yum_InitSubsystem(const char *homedir) {
  if (g_Resource) {
    return {1, YUM_UID_NULL};
  }

  try {
    g_Resource = new Resource(0x1000, homedir);
    return {0, YUM_UID_NULL};
  } catch (...) {
    return {1, YUM_UID_NULL};
  }
}

YumErr Yum_NewWorld() {
  if (g_Resource) {
    return YumErr {
      .code = 0,
      .uid = g_Resource->createWorld()
    };
  }

  return YUM_NOT_READY;
}

YumErr Yum_Compile(const char *src, uint8_t libs) {
  if (g_Resource) return g_Resource->compile(src, true, libs);

  return YUM_NOT_READY;
}

YumErr Yum_CompileString(const char *src, uint8_t libs) {
  if (g_Resource) return g_Resource->compile(src, false, libs);

  return YUM_NOT_READY;
}

void Yum_DestroyWorld(Yumuid world) {
  if (g_Resource) g_Resource->destroyWorld(world);
}

YumErr Yum_RunCode(Yumuid world, Yumuid src) {
  if (g_Resource) return g_Resource->run(world, src);

  return YUM_NOT_READY;
}

const char *Yum_GetError(YumErr e) {
  if (g_Resource) return g_Resource->getError(e.uid).c_str();

  return "err: fatal: Yum: not init!";
}

void Yum_Shutdown() {
  if (g_Resource) {
    delete g_Resource;
  }
  
  g_Resource = nullptr;
}