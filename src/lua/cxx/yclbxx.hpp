#pragma once

#include <string>
#include <vector>
#include <format> // FORMAT IS SO FUCKING GOOD LOLLL
#include <cstdint>
#include <variant>
#include <stdexcept>
#include <functional>

#include <lua.hpp>

namespace Yum {
  namespace Yum_cxx {
    /*Do NOT bind Y_lua2cxx function DIRECTLY to C-extern — they can throw! */
    namespace Y_lua2cxx {
      using lua2cxxT = std::variant<double, int64_t, std::string, bool>;

      struct lua2cxx_argv {
        std::vector<lua2cxxT> args;
      };

      class lua2cxx_func {
      private:
        std::string fname;
        int retc;
        std::function<std::vector<lua2cxxT>(const lua2cxx_argv&)> cxx_callback;

      public:
        inline lua2cxx_func(std::string n, int r,
                    std::function<std::vector<lua2cxxT>(const lua2cxx_argv&)> cb)
            : fname(std::move(n)), retc(r), cxx_callback(std::move(cb)) {}

        inline int interface(lua_State *L) {
          if (!cxx_callback) {
            throw std::runtime_error(std::format(
              "{}.{}.{}(): cxx_callback is null.",
              typeid(*this).name(), fname, __func__));
          }

          lua2cxx_argv argv;
          int nargs = lua_gettop(L);
          for (int i = 1; i <= nargs; i++) {
            if (lua_isinteger(L, i)) argv.args.push_back((int64_t)lua_tointeger(L, i));
            else if (lua_isnumber(L, i)) argv.args.push_back(lua_tonumber(L, i));
            else if (lua_isboolean(L, i)) argv.args.push_back((bool)lua_toboolean(L, i));
            else if (lua_isstring(L, i)) argv.args.push_back(std::string(lua_tostring(L, i)));
            else argv.args.push_back(std::string(luaL_typename(L, i)));
          }

          auto results = cxx_callback(argv);

          for (auto &v : results) {
            if (auto p = std::get_if<int64_t>(&v)) lua_pushinteger(L, *p);
            else if (auto p = std::get_if<double>(&v)) lua_pushnumber(L, *p);
            else if (auto p = std::get_if<bool>(&v)) lua_pushboolean(L, *p);
            else if (auto p = std::get_if<std::string>(&v)) lua_pushstring(L, p->c_str());
          }

          return (int)results.size();
        }

        inline const std::string &name() const { return fname; }
      };
    }
  }
}
