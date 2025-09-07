#include <stack>
#include <format>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <cstdint>
#include <variant>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include <lua.hpp>

#include "yuxx.h"
#include "yclbxx.hpp"
#include "../yumv.h"

namespace Yum {
  namespace Yum_cxx {
    class YumLua {
    private:
      std::string makeError(const std::string &what, const std::string &from) {
        using namespace std::chrono;

        auto now = system_clock::now();
        auto in_time_t = system_clock::to_time_t(now);
        auto us = duration_cast<microseconds>(now.time_since_epoch()) % 1000000;

        std::ostringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%H:%M:%S")
          << "." << std::setfill('0') << std::setw(6) << us.count();


        std::ostringstream out;
        out << ":" << ss.str() << ": "
            << typeid(this).name() << "." << from << "(): "
            << what;

        return out.str();
      }

      lua_State  *G_State;
      bool        cleaned = false;
      std::stack<std::string> errorStack;
      std::vector<std::variant<int64_t, double, bool, std::string>> argvVec;
      std::unordered_map<std::string, std::shared_ptr<Y_lua2cxx::lua2cxx_func>> registeredCallbacks;
      std::unordered_map<std::string, std::shared_ptr<YumCSharpFunc>> CSCallbacks;

      void pushStack(int64_t i) { lua_pushinteger(G_State, i); }
      void pushStack(double d) { lua_pushnumber(G_State, d); }
      void pushStack(const std::string &s) { lua_pushstring(G_State, s.c_str()); }
      void pushStack(bool b) { lua_pushboolean(G_State, b); }

    public:
      YumLua() : G_State(nullptr), cleaned(false), errorStack() {}
      ~YumLua() {
        if (!cleaned) {
          errorStack.push(makeError("(WARN) destroying uncleaned object! --resrouces may leaked", __func__));
          if (G_State) lua_close(G_State);
        }

        std::cout << "yum: error(s) occured during the runtime: (most recent down)" << std::endl;
        while (!errorStack.empty()) {
          std::cout << "yum:\t* " << errorStack.top() << std::endl;
          errorStack.pop();
        }
      }

      int32_t initSubsystem() {
        G_State = luaL_newstate();

        if (!G_State) {
          errorStack.push(makeError("unable to create lua_State", __func__));
          std::cerr << errorStack.top() << std::endl;
          return -1;
        }

        luaL_openlibs(G_State);

        return 0;
      }

      void pushArgv(int64_t a) { argvVec.push_back(a); }
      void pushArgv(double a) { argvVec.push_back(a); }
      void pushArgv(const std::string &a) { argvVec.push_back(a); }
      void pushArgv(bool a) { argvVec.push_back(a); }

      int32_t call(const std::string &name) {
        if (!G_State) return -1;
        lua_getglobal(G_State, name.c_str());

        int nargs = argvVec.size();
        for (auto &arg : argvVec)
          std::visit([this](auto&& value){ pushStack(value); }, arg);

        if (lua_pcall(G_State, nargs, LUA_MULTRET, 0) != LUA_OK) {
          errorStack.push(makeError(lua_tostring(G_State, -1), __func__));
          return -1;
        }

        argvVec.clear();
        return 0;
      }

      int32_t load(const std::string &src, bool string = true) {
        if (((string) ? luaL_loadstring(G_State, src.c_str())
                      : luaL_loadfile(G_State, src.c_str())) 
                      != LUA_OK) {
          errorStack.push(makeError(lua_tostring(G_State, -1), __func__));
          return -1;
        }

        if (lua_pcall(G_State, 0, LUA_MULTRET, 0) != LUA_OK) {
          errorStack.push(makeError(lua_tostring(G_State, -1), __func__));
          return -1;
        }

        return 0;
      }

      void registerFunction(const std::shared_ptr<Y_lua2cxx::lua2cxx_func> &func) {
        if (!G_State) {
          errorStack.push(makeError("registerFunction() called before initSubsystem()!", __func__));
          return;
        }

        registeredCallbacks[func->name()] = func;

        lua_pushlightuserdata(G_State, func.get());
        try {
          lua_pushcclosure(G_State, [](lua_State *L) -> int {
            auto* f = (Y_lua2cxx::lua2cxx_func*)lua_touserdata(L, lua_upvalueindex(1));
            return f->interface(L);
          }, 1);

          lua_setglobal(G_State, func->name().c_str());
        } catch (const std::exception &e) {
          /* Since it's a C-driven API, and that Y_lua2cxx::lua2cxx_func can throw, we must handle exceptions. */
          errorStack.push(makeError(std::string("caught exception: ") + e.what(), __func__));
        }
        /* That region is really unsafe (I guess!). I putted the setglobal() in the try block, 
         ** so in the case an exception is thrown, Lua won't register the function.
         ** But still, we need to be careful, 'caus we can still endup with 'attempt to call nil value' in Lua.
         ** Generally it won't cause any problem. But we need to keep careful. 
         */
      }

      int32_t registerCSCallback(const std::string &name, const std::shared_ptr<YumCSharpFunc> &func) {
        if (!G_State) {
          errorStack.push(makeError(std::string(__func__) + "() called before initSubsystem()!", __func__));
          return -1;
        }

        // keep ownership in map
        CSCallbacks[name] = func;

        // push pointer to the stored YumCSharpFunc (raw pointer from shared_ptr)
        lua_pushlightuserdata(G_State, func.get());

        try {
          // create a Lua C closure that will call the C# callback when invoked from Lua
          lua_pushcclosure(G_State, [](lua_State *L) -> int {
            // retrieve the YumCSharpFunc pointer we stored as upvalue
            auto* fptr = (YumCSharpFunc*)lua_touserdata(L, lua_upvalueindex(1));
            if (!fptr) {
              lua_pushstring(L, "Yum: invalid C# callback pointer");
              lua_error(L);
              return 0; // unreachable, lua_error long-jumps
            }

            // build args vector from Lua stack
            int nargs = lua_gettop(L);
            std::vector<YcxxVariantBase> args;
            args.reserve(nargs);

            for (int i = 1; i <= nargs; ++i) {
              int t = lua_type(L, i);
              switch (t) {
                case LUA_TNUMBER:
                  if (lua_isinteger(L, i)) {
                    args.emplace_back((int64_t)lua_tointeger(L, i));
                  } else {
                    args.emplace_back((double)lua_tonumber(L, i));
                  }
                  break;
                case LUA_TBOOLEAN:
                  args.emplace_back((bool)lua_toboolean(L, i));
                  break;
                case LUA_TSTRING:
                  args.emplace_back(std::string(lua_tostring(L, i)));
                  break;
                default:
                  // unsupported type -> push nil and return with an error message
                  lua_pushnil(L);
                  lua_pushfstring(L, "Yum: unsupported arg type for index %d", i);
                  return 2;
              }
            }

            // call the C# function (wrapped in YumCSharpFunc)
            try {
              auto Vargs = cxxVecVariant2CVecVariant(args);
              auto result = (*fptr)(Vargs);
              YcxxYum_deleteVec(Vargs); /* BAD CODE — imma try to make it proper! */

              // push result onto Lua stack according to its held type
              std::visit([L](auto&& value){
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, int64_t>) {
                  lua_pushinteger(L, value);
                } else if constexpr (std::is_same_v<T, double>) {
                  lua_pushnumber(L, value);
                } else if constexpr (std::is_same_v<T, bool>) {
                  lua_pushboolean(L, value);
                } else if constexpr (std::is_same_v<T, std::string>) {
                  lua_pushstring(L, value.c_str());
                } else {
                  lua_pushnil(L);
                }
              }, CVariant2CxxVariant(result));

              return 1; // one return value
            } catch (const std::exception &e) {
              lua_pushnil(L);
              lua_pushfstring(L, "Yum: exception from C# callback: %s", e.what());
              return 2;
            }
          }, 1);

          // register the closure as a global with the given name
          lua_setglobal(G_State, name.c_str());
        } catch (const std::exception &e) {
          errorStack.push(makeError(std::string("caught exception: ") + e.what(), __func__));
          return -1;
        }

        return 0;
      }


      void cleanUp() {
//        if (G_State) {
//          delete G_State;
//          G_State = nullptr;
//        }

//        cleaned = true;
      }
    };
  } 
}

static Yum::Yum_cxx::YumLua *G_YumIstance;

extern "C" {
  int32_t YcxxYum_initSubsystem() {
    std::cout << 
      std::format("YumEngine.{}.{}.{}.{}.{} - Lua.Runtime.{}.{}", 
        YUM_STUDIO, YUM_BRANCH, YUM_VERSION_MAJOR, YUM_VERSION_MINOR, YUM_VERSION_PATCH, 
        LUA_VERSION_MAJOR, LUA_VERSION_MINOR
      ) << std::endl;
    
    G_YumIstance = new Yum::Yum_cxx::YumLua();
    if (!G_YumIstance) return -1;
    return G_YumIstance->initSubsystem();
  }

  void YcxxYum_shutdownSubsystem() {
    if (G_YumIstance) {
      G_YumIstance->cleanUp();
      delete G_YumIstance;
      G_YumIstance = nullptr;
    }
  }

  void YcxxYum_pushInt(int64_t a) { 
    if (G_YumIstance) {
      G_YumIstance->pushArgv(a);
    }
  }

  void YcxxYum_pushNumber(double a) { 
    if (G_YumIstance) {
      G_YumIstance->pushArgv(a);
    }
  }

  void YcxxYum_pushString(const char *a) {
    if (G_YumIstance) {
      G_YumIstance->pushArgv(std::string(a));
    }
  }

  void YcxxYum_pushBoolean(int a) {
    if (G_YumIstance) {
      G_YumIstance->pushArgv((bool)a);
    }
  }

  int32_t YcxxYum_loadString(const char *str) {
    if (G_YumIstance) {
      return G_YumIstance->load(str, true);
    } return -1;
  }

  int32_t YcxxYum_loadFile(const char *src) {
    if (G_YumIstance) {
      return G_YumIstance->load(src, false);
    } return -1;
  }

  int32_t YcxxYum_call(const char *name) {
    if (G_YumIstance) {
      return G_YumIstance->call(name);
    } return -1;
  }

  int32_t YcxxYum_registerCSCallback(const char *name, YumCSharpFunc func) {
    if (!G_YumIstance) return -1;
    if (!name || !func) return -1;

    auto sp = std::shared_ptr<YumCSharpFunc>(
      new YumCSharpFunc(func),
      [](YumCSharpFunc* p) {
        std::cout << "yum: [Yum.Debug] Destroying C# callback at " << p << std::endl;
        delete p;
      }
    );

    std::cout << "yum: [Yum.Debug] Copied and registered C# callback: " << name 
              << " at " << sp.get() << std::endl;

    return G_YumIstance->registerCSCallback(std::string(name), sp);
  }

}