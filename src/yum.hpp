#pragma once

#include <lua.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <lua.hpp>
#include <filesystem>
#include <unordered_map>
#include <chrono>

#define YUMDIR ".YumYum"
#define YUMBIN ".YumYum/bin"
#define YUMCFG ".YumYum/cache.cfg"

namespace Yum {
  inline static int writer(lua_State *L, const void *p, size_t sz, void *ud) {
    std::ofstream *out = reinterpret_cast<std::ofstream*>(ud);
    out->write(reinterpret_cast<const char*>(p), sz);
    return 0;
  }

  // Ex: Path: {"Game", "Player"}
  inline void ensureNestedTable(lua_State *L, const std::vector<std::string> &path) {
    for (size_t i = 0; i < path.size(); ++i) {
      if (i == 0) {
        lua_getglobal(L, path[0].c_str());
        if (!lua_istable(L, -1)) {
          lua_pop(L, 1);
          lua_newtable(L);
          lua_setglobal(L, path[0].c_str());
          lua_getglobal(L, path[0].c_str());
        }
      } else {
        lua_getfield(L, -1, path[i].c_str());
        if (!lua_istable(L, -1)) {
          lua_pop(L, 1);
          lua_newtable(L);
          lua_setfield(L, -2, path[i].c_str());
          lua_getfield(L, -1, path[i].c_str());
        }
        lua_remove(L, -2); 
      }
    }
  }

  inline void addFunctionToNestedTable(lua_State *L, const std::vector<std::string> &path, const std::string &funcName, lua_CFunction fn) {
    ensureNestedTable(L, path);
    lua_pushcfunction(L, fn);
    lua_setfield(L, -2, funcName.c_str());
    lua_pop(L, 1);
  }

  inline int compile(const std::string &path, const std::string &output) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_loadfile(L, path.c_str()) != LUA_OK) {
      std::cerr << "err: Yum.Lua: " << lua_tostring(L, -1) << "\n";
      lua_close(L);
      return 1;
    }

    std::ofstream out(output, std::ios::binary);
    if (!out) {
      std::cerr << "err: Yum.Lua: Failed to open output file\n";
      lua_close(L);
      return 1;
    }

    if (lua_dump(L, writer, &out, 0) != 0) {
      std::cerr << "err: Yum.Lua: lua_dump failed!\n";
      lua_close(L);
      return 1;
    }

    lua_close(L);
    return 0;
  }

  using CacheMap = std::unordered_map<std::string, std::string>;

  inline uint64_t newUID() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
  }

  inline CacheMap loadCacheConfig(const std::string &cfgFile) {
    CacheMap cache;
    std::ifstream in(cfgFile);
    if (!in) return cache;

    std::string line;
    while (std::getline(in, line)) {
      auto pos = line.find('=');
      if (pos == std::string::npos) continue;
      std::string src = line.substr(0, pos);
      std::string bin = line.substr(pos + 1);
      cache[src] = bin;
    }
    return cache;
  }

  inline void saveCacheConfig(const std::string &cfgFile, const CacheMap &cache) {
    std::ofstream out(cfgFile, std::ios::trunc);
    for (const auto &[src, bin] : cache) {
      out << src << "=" << bin << "\n";
    }
  }

  inline bool needsRecompile(const std::string &src, const std::string &bin) {
    if (!std::filesystem::exists(bin)) return true;
    auto srcTime = std::filesystem::last_write_time(src);
    auto binTime = std::filesystem::last_write_time(bin);
    return binTime < srcTime;
  }

  inline std::string getCachedFile(CacheMap &cache, const std::string &src, bool forceRecompile) {
    std::string bin;

    auto it = cache.find(src);
    if (it == cache.end()) {
      bin = std::to_string(newUID()) + ".yumcc";
      cache[src] = bin;
      forceRecompile = true;
    } else {
      bin = it->second;
    }

    std::string binPath = std::string(YUMBIN) + "/" + bin;
    if (forceRecompile || needsRecompile(src, binPath)) {
      std::cout << "=> Yum.Lua: [compiling] " << src << " -> " << binPath << "\n";
      Yum::compile(src, binPath);
    }

    return binPath;
  }

  inline bool runLuaFile(lua_State *L, const std::string &file) {
    if (luaL_loadfile(L, file.c_str()) != LUA_OK || lua_pcall(L, 0, 0, 0) != LUA_OK) {
      std::cerr << "=> err: " << file << " -> " << lua_tostring(L, -1) << "\n";
      lua_pop(L, 1);
      return false;
    }
    return true;
  }

}