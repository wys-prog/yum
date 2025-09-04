#include <lua.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <chrono>

#include "yum.hpp"

int main(int argc, char *const argv[]) {
  std::filesystem::create_directories(YUMBIN);

  bool forceAll = (argc > 1 && std::string(argv[1]) == "-a");

  Yum::CacheMap cache = Yum::loadCacheConfig(YUMCFG);

  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  if (luaL_loadfile(L, "yum.lua") || lua_pcall(L, 0, 1, 0)) {
    std::cerr << "=> err: Yum.Lua: " << lua_tostring(L, -1) << "\n";
    lua_close(L);
    return 1;
  }
  if (!lua_istable(L, -1)) {
    std::cerr << "=> err: yum.lua did not return a table!\n";
    lua_close(L);
    return 1;
  }

  lua_getfield(L, -1, "includes");
  if (!lua_istable(L, -1)) {
    std::cerr << "=> err: 'includes' field missing or not a table\n";
    lua_close(L);
    return 1;
  }

  size_t len = lua_rawlen(L, -1);
  for (size_t i = 1; i <= len; ++i) {
    lua_rawgeti(L, -1, i);
    if (!lua_isstring(L, -1)) {
      std::cerr << "=> err: include #" << i << " is not a string!\n";
      lua_pop(L, 1);
      continue;
    }
    std::string file = lua_tostring(L, -1);
    lua_pop(L, 1);

    std::string binPath = Yum::getCachedFile(cache, file, forceAll);
    Yum::runLuaFile(L, binPath);
  }

  lua_pop(L, 1);

  lua_getfield(L, -1, "main");
  std::string mainFile = lua_tostring(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "output");
  std::string outputFile = lua_tostring(L, -1);
  lua_pop(L, 1);

  std::cout << "=> Yum.Lua: Compiling main: " << mainFile << " - " << outputFile << "...\n";
  Yum::compile(mainFile, outputFile);
  Yum::runLuaFile(L, outputFile);

  Yum::saveCacheConfig(YUMCFG, cache);

  lua_close(L);
  return 0;
}
