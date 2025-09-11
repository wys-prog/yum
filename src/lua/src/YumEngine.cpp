#include <string>
#include <stdexcept>
#include <iostream>
#include <memory>

#if defined(_WIN32) || defined(_WIN64)
  #include <windows.h>
  using LibHandle = HMODULE;
#else
  #include <dlfcn.h>
  using LibHandle = void*;
#endif

class DynamicLibrary {
public:
    explicit DynamicLibrary(const std::string& name) {
        handle = load(name);
        if (!handle) {
            throw std::runtime_error("Failed to load library: " + name);
        }
    }

    ~DynamicLibrary() {
        if (handle) {
            close(handle);
        }
    }

    // Non-copyable
    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;

    // Movable
    DynamicLibrary(DynamicLibrary&& other) noexcept : handle(other.handle) {
        other.handle = nullptr;
    }
    DynamicLibrary& operator=(DynamicLibrary&& other) noexcept {
        if (this != &other) {
            if (handle) close(handle);
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }

    template<typename T>
    T getSymbol(const std::string& symbol) {
        void* sym = resolve(handle, symbol);
        if (!sym) {
            throw std::runtime_error("Failed to resolve symbol: " + symbol);
        }
        return reinterpret_cast<T>(sym);
    }

private:
    LibHandle handle{nullptr};

    static LibHandle load(const std::string& name) {
#if defined(_WIN32) || defined(_WIN64)
        std::string file = name;
        if (!hasExtension(file)) file += ".dll";
        return LoadLibraryA(file.c_str());
#elif defined(__APPLE__)
        std::string file = name;
        if (!hasExtension(file)) file += ".dylib";
        return dlopen(file.c_str(), RTLD_LAZY);
#else // Linux / Unix
        std::string file = name;
        if (!hasExtension(file)) file += ".so";
        return dlopen(file.c_str(), RTLD_LAZY);
#endif
    }

    static void close(LibHandle h) {
#if defined(_WIN32) || defined(_WIN64)
        FreeLibrary(h);
#else
        dlclose(h);
#endif
    }

    static void* resolve(LibHandle h, const std::string& symbol) {
#if defined(_WIN32) || defined(_WIN64)
        return reinterpret_cast<void*>(GetProcAddress(h, symbol.c_str()));
#else
        return dlsym(h, symbol.c_str());
#endif
    }

    static bool hasExtension(const std::string& file) {
        return file.find('.') != std::string::npos;
    }
};


//
// ============================
//  YumAPI Loader
// ============================
//
struct YumAPI {
    using YumSubsystem_new_t       = void* (*)();
    using YumSubsystem_delete_t    = void  (*)(void*);
    using YumSubsystem_newState_t  = uint64_t (*)(void*);
    using YumSubsystem_deleteState_t = void (*)(void*, uint64_t);
    using YumSubsystem_isValidUID_t = int32_t (*)(void*, uint64_t);
    using YumLuaSubsystem_load_t   = int32_t (*)(void*, uint64_t, const char*, bool);
    using YumLuaSubsystem_good_t   = bool    (*)(void*, uint64_t);
    using YumLuaSubsystem_call_t   = void*   (*)(void*, uint64_t, const char*, const void*);

    using YumVector_new_t          = void* (*)();
    using YumVector_delete_t       = void  (*)(void*);
    using YumVector_append_t       = void  (*)(void*, const void*);
    using YumVector_clear_t        = void  (*)(void*);
    using YumVector_size_t         = int64_t (*)(const void*);
    using YumVector_at_t           = void* (*)(void*, int64_t);
    using YumVector_at_const_t     = const void* (*)(const void*, int64_t);

    using YumVariant_new_t         = void* (*)();
    using YumVariant_delete_t      = void  (*)(void*);
    using YumVariant_setInt_t      = void  (*)(void*, int64_t);
    using YumVariant_setFloat_t    = void  (*)(void*, double);
    using YumVariant_setBool_t     = void  (*)(void*, int);
    using YumVariant_setString_t   = void  (*)(void*, const char*);

    using YumVariant_asInt_t       = int64_t (*)(const void*);
    using YumVariant_asFloat_t     = double  (*)(const void*);
    using YumVariant_asBool_t      = int     (*)(const void*);
    using YumVariant_asString_t    = const char* (*)(const void*);

    using YumVariant_isInt_t       = int (*)(const void*);
    using YumVariant_isFloat_t     = int (*)(const void*);
    using YumVariant_isBool_t      = int (*)(const void*);
    using YumVariant_isString_t    = int (*)(const void*);

    // Function pointers
    YumSubsystem_new_t YumSubsystem_new;
    YumSubsystem_delete_t YumSubsystem_delete;
    YumSubsystem_newState_t YumSubsystem_newState;
    YumSubsystem_deleteState_t YumSubsystem_deleteState;
    YumSubsystem_isValidUID_t YumSubsystem_isValidUID;
    YumLuaSubsystem_load_t YumLuaSubsystem_load;
    YumLuaSubsystem_good_t YumLuaSubsystem_good;
    YumLuaSubsystem_call_t YumLuaSubsystem_call;

    YumVector_new_t YumVector_new;
    YumVector_delete_t YumVector_delete;
    YumVector_append_t YumVector_append;
    YumVector_clear_t YumVector_clear;
    YumVector_size_t YumVector_size;
    YumVector_at_t YumVector_at;
    YumVector_at_const_t YumVector_at_const;

    YumVariant_new_t YumVariant_new;
    YumVariant_delete_t YumVariant_delete;
    YumVariant_setInt_t YumVariant_setInt;
    YumVariant_setFloat_t YumVariant_setFloat;
    YumVariant_setBool_t YumVariant_setBool;
    YumVariant_setString_t YumVariant_setString;
    YumVariant_asInt_t YumVariant_asInt;
    YumVariant_asFloat_t YumVariant_asFloat;
    YumVariant_asBool_t YumVariant_asBool;
    YumVariant_asString_t YumVariant_asString;
    YumVariant_isInt_t YumVariant_isInt;
    YumVariant_isFloat_t YumVariant_isFloat;
    YumVariant_isBool_t YumVariant_isBool;
    YumVariant_isString_t YumVariant_isString;

    explicit YumAPI(DynamicLibrary& lib) {
        // Subsystem
        YumSubsystem_new        = lib.getSymbol<YumSubsystem_new_t>("YumSubsystem_new");
        YumSubsystem_delete     = lib.getSymbol<YumSubsystem_delete_t>("YumSubsystem_delete");
        YumSubsystem_newState   = lib.getSymbol<YumSubsystem_newState_t>("YumSubsystem_newState");
        YumSubsystem_deleteState= lib.getSymbol<YumSubsystem_deleteState_t>("YumSubsystem_deleteState");
        YumSubsystem_isValidUID = lib.getSymbol<YumSubsystem_isValidUID_t>("YumSubsystem_isValidUID");
        YumLuaSubsystem_load    = lib.getSymbol<YumLuaSubsystem_load_t>("YumLuaSubsystem_load");
        YumLuaSubsystem_good    = lib.getSymbol<YumLuaSubsystem_good_t>("YumLuaSubsystem_good");
        YumLuaSubsystem_call    = lib.getSymbol<YumLuaSubsystem_call_t>("YumLuaSubsystem_call");

        // Vector
        YumVector_new           = lib.getSymbol<YumVector_new_t>("YumVector_new");
        YumVector_delete        = lib.getSymbol<YumVector_delete_t>("YumVector_delete");
        YumVector_append        = lib.getSymbol<YumVector_append_t>("YumVector_append");
        YumVector_clear         = lib.getSymbol<YumVector_clear_t>("YumVector_clear");
        YumVector_size          = lib.getSymbol<YumVector_size_t>("YumVector_size");
        YumVector_at            = lib.getSymbol<YumVector_at_t>("YumVector_at");
        YumVector_at_const      = lib.getSymbol<YumVector_at_const_t>("YumVector_at_const");

        // Variant
        YumVariant_new          = lib.getSymbol<YumVariant_new_t>("YumVariant_new");
        YumVariant_delete       = lib.getSymbol<YumVariant_delete_t>("YumVariant_delete");
        YumVariant_setInt       = lib.getSymbol<YumVariant_setInt_t>("YumVariant_setInt");
        YumVariant_setFloat     = lib.getSymbol<YumVariant_setFloat_t>("YumVariant_setFloat");
        YumVariant_setBool      = lib.getSymbol<YumVariant_setBool_t>("YumVariant_setBool");
        YumVariant_setString    = lib.getSymbol<YumVariant_setString_t>("YumVariant_setString");
        YumVariant_asInt        = lib.getSymbol<YumVariant_asInt_t>("YumVariant_asInt");
        YumVariant_asFloat      = lib.getSymbol<YumVariant_asFloat_t>("YumVariant_asFloat");
        YumVariant_asBool       = lib.getSymbol<YumVariant_asBool_t>("YumVariant_asBool");
        YumVariant_asString     = lib.getSymbol<YumVariant_asString_t>("YumVariant_asString");
        YumVariant_isInt        = lib.getSymbol<YumVariant_isInt_t>("YumVariant_isInt");
        YumVariant_isFloat      = lib.getSymbol<YumVariant_isFloat_t>("YumVariant_isFloat");
        YumVariant_isBool       = lib.getSymbol<YumVariant_isBool_t>("YumVariant_isBool");
        YumVariant_isString     = lib.getSymbol<YumVariant_isString_t>("YumVariant_isString");
    }
};

//
// ============================
//  RAII Wrappers
// ============================
//

class Variant {
    YumAPI& api;
    void* handle;
public:
    explicit Variant(YumAPI& api_) : api(api_), handle(api.YumVariant_new()) {}
    ~Variant() { api.YumVariant_delete(handle); }

    void set(int64_t v) { api.YumVariant_setInt(handle, v); }
    void set(double v)  { api.YumVariant_setFloat(handle, v); }
    void set(bool v)    { api.YumVariant_setBool(handle, v ? 1 : 0); }
    void set(const std::string& s) { api.YumVariant_setString(handle, s.c_str()); }

    int64_t asInt()   const { return api.YumVariant_asInt(handle); }
    double  asFloat() const { return api.YumVariant_asFloat(handle); }
    bool    asBool()  const { return api.YumVariant_asBool(handle) != 0; }
    std::string asString() const { return api.YumVariant_asString(handle); }

    bool isInt()    const { return api.YumVariant_isInt(handle); }
    bool isFloat()  const { return api.YumVariant_isFloat(handle); }
    bool isBool()   const { return api.YumVariant_isBool(handle); }
    bool isString() const { return api.YumVariant_isString(handle); }

    void* raw() { return handle; }
    const void* raw() const { return handle; }
};

class Vector {
    YumAPI& api;
    void* handle;
public:
    explicit Vector(YumAPI& api_) : api(api_), handle(api.YumVector_new()) {}
    ~Vector() { api.YumVector_delete(handle); }

    void append(const Variant& v) { api.YumVector_append(handle, v.raw()); }
    void clear() { api.YumVector_clear(handle); }
    int64_t size() const { return api.YumVector_size(handle); }

    Variant at(int64_t i) {
        void* raw = api.YumVector_at(handle, i);
        // Wrap existing Variant pointer (could extend this!)
        Variant v(api);
        // Danger: right now, creating new instead of aliasing raw. Could be improved.
        return v;
    }

    void* raw() { return handle; }
    const void* raw() const { return handle; }
};

class Subsystem {
    YumAPI& api;
    void* handle;
public:
    explicit Subsystem(YumAPI& api_) : api(api_), handle(api.YumSubsystem_new()) {}
    ~Subsystem() { api.YumSubsystem_delete(handle); }

    uint64_t newState() { return api.YumSubsystem_newState(handle); }
    void deleteState(uint64_t uid) { api.YumSubsystem_deleteState(handle, uid); }
    bool isValid(uint64_t uid) { return api.YumSubsystem_isValidUID(handle, uid); }

    int load(uint64_t uid, const std::string& src, bool isFile) {
        return api.YumLuaSubsystem_load(handle, uid, src.c_str(), isFile);
    }

    bool good(uint64_t uid) { return api.YumLuaSubsystem_good(handle, uid); }

    Vector call(uint64_t uid, const std::string& name, const Vector& args) {
        void* res = api.YumLuaSubsystem_call(handle, uid, name.c_str(), args.raw());
        // In real engine: wrap returned YumVector* into Vector wrapper.
        return Vector(api);
    }
};

//
// ============================
//  Example Main
// ============================
//
int main() {
    try {
        DynamicLibrary lib("yum"); // yum.so / yum.dylib / yum.dll
        YumAPI api(lib);

        Subsystem sys(api);
        uint64_t uid = sys.newState();
        sys.load(uid, "print('hello from Lua')", false);

        Variant v(api);
        v.set((int64_t)42);
        std::cout << "Variant = " << v.asInt() << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "[Error] " << e.what() << "\n";
    }
}
