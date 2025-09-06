#include <stack>
#include <format>
#include <vector>
#include <string>
#include <variant>
#include <cstdint>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

#include <lua.hpp>

#include "yuxx.h"
#include "../yumv.h"

namespace Yum {
  static int luaPrint(lua_State* L) {
    int n = lua_gettop(L);

    for (int i = 1; i <= n; i++) {
      if (lua_isstring(L, i)) {
        std::cout << lua_tostring(L, i);
      } else if (lua_isnumber(L, i)) {
        std::cout << lua_tonumber(L, i);
      } else if (lua_isboolean(L, i)) {
        std::cout << (lua_toboolean(L, i) ? "true" : "false");
      } else {
        std::cout << luaL_typename(L, i);
      }
      if (i < n) std::cout << "\t";
    }

    std::cout << std::endl;

    return 0;
  }

  void registerPrint(lua_State* L) {
    lua_pushcfunction(L, luaPrint);
    lua_setglobal(L, "print");
  }

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

      void pushStack(int64_t i) { lua_pushinteger(G_State, i); }
      void pushStack(double d) { lua_pushnumber(G_State, d); }
      void pushStack(const std::string &s) { lua_pushstring(G_State, s.c_str()); }
      void pushStack(bool b) { lua_pushboolean(G_State, b); }

    public:
      YumLua() : G_State(nullptr), cleaned(false), errorStack() {}
      ~YumLua() {
        if (!cleaned) {
          errorStack.push(makeError("destroying uncleaned object! --resrouces may leaked", __func__));
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
        registerPrint(G_State);

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
      G_YumIstance->pushArgv(a);
    }
  }

  void YcxxYum_pushBoolean(int16_t a) {
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
}