---@diagnostic disable: need-check-nil

-- ========================
-- CONFIGURATION (easy to edit)
-- ========================
local studio_name = "yum_official"         -- studio building this
local branch_name = "main"                 -- git branch or any string
local CXX = "g++-15"                       -- C++ compiler
local CC  = "gcc-15"                       -- C compiler
local LUA_INC = "/opt/homebrew/include/lua5.4"
local LUA_LIB = "/opt/homebrew/lib"
local BUILD_DIR = "tests/bin"
local OUT_LIB = BUILD_DIR .. "/../yum.dylib"
local CHECK_CMD = "otool -L"
local version_file = "src/lua/yumv.h"

-- === SOURCES ===
local SRC_CXX = {
    "src/lua/cxx/yuxx.cpp",
    "src/lua/cxx/variant.cpp",
    "src/lua/cxx/vec.cpp",
}
local SRC_C = {
    "src/lua/yum.c",
}

-- === INCLUDES / LIBS ===
local INCLUDES = {
    LUA_INC,
    "src/lua/cxx",
}
local LIBS = {
    "-llua", "-lm", "-ldl"
}

-- ========================
-- HELPERS
-- ========================
local function read_file(path)
    local f = io.open(path, "r")
    if not f then return nil end
    local data = f:read("*a")
    f:close()
    return data
end

local function write_file(path, data)
    local f = io.open(path, "w")
    assert(f, "cannot write " .. path)
    f:write(data)
    f:close()
end

-- bump version
local function bump_version(kind)
    local content = read_file(version_file)
    assert(content, "version file not found!")

    local major, minor, patch =
        content:match("#%s*define%s+YUM_VERSION%s+YUM_VERSION_ENCODE%((%d+),%s*(%d+),%s*(%d+)%)")

    assert(major and minor and patch, "failed to parse version!")

    major = tonumber(major)
    minor = tonumber(minor)
    patch = tonumber(patch)

    if kind == "major" then
        major = major + 1
        minor = 0
        patch = 0
    elseif kind == "minor" then
        minor = minor + 1
        patch = 0
    else
        patch = patch + 1
    end

    -- replace version line
    local new_define = string.format("#   define YUM_VERSION YUM_VERSION_ENCODE(%d, %d, %d)", major, minor, patch)
    local updated = content:gsub("#%s*define%s+YUM_VERSION%s+YUM_VERSION_ENCODE%([^%)]+%)", new_define)

    -- inject/update macros for studio and branch
    if updated:match("#%s*define%s+YUM_STUDIO") then
        updated = updated:gsub("#%s*define%s+YUM_STUDIO%s+\"[^\"]*\"", "#   define YUM_STUDIO \"" .. studio_name .. "\"")
    else
        updated = "#   define YUM_STUDIO \"" .. studio_name .. "\"\n" .. updated
    end

    if updated:match("#%s*define%s+YUM_BRANCH") then
        updated = updated:gsub("#%s*define%s+YUM_BRANCH%s+\"[^\"]*\"", "# define YUM_BRANCH \"" .. branch_name .. "\"")
    else
        updated = "# define YUM_BRANCH \"" .. branch_name .. "\"\n" .. updated
    end

    write_file(version_file, updated)
    return string.format("%d.%d.%d", major, minor, patch)
end

-- detect toolchain
local function detect_toolchain()
    local gxx_handle = io.popen(CXX .. " --version")
    local gxx = gxx_handle:read("*l") or "unknown g++"
    gxx_handle:close()

    local gcc_handle = io.popen(CC .. " --version")
    local gcc = gcc_handle:read("*l") or "unknown gcc"
    gcc_handle:close()

    return gxx, gcc
end

-- run shell commands
local function run(cmd)
    print("\t> " .. cmd)
    local handle = io.popen(cmd .. " 2>&1")
    local result = handle:read("*a")
    local success, _, code = handle:close()
    if code ~= 0 then
        error("Command failed with code " .. code .. "\nOutput:\n" .. result)
    end
end

-- join helpers
local function join(tbl, sep)
    local buf = {}
    for _, v in ipairs(tbl) do table.insert(buf, v) end
    return table.concat(buf, sep)
end

local function make_includes()
    local buf = {}
    for _, inc in ipairs(INCLUDES) do
        table.insert(buf, '-I"' .. inc .. '"')
    end
    return table.concat(buf, " ")
end

-- ========================
-- MAIN BUILD
-- ========================
local function main()
    print("==== Yum Build System ====")
    print("[1/6] Parsing...")
    local kind = parse_args()
    print("[2/6] reading version...")
    local ver = bump_version(kind)
    print("[3/6] detecting toolchain...")
    local gxx, gcc = detect_toolchain()

    print("\t|- Version: " .. ver)
    print("\t|- Studio: " .. studio_name)
    print("\t|- Branch: " .. branch_name)
    print("\t|- Toolchain:")
    print("\t|-   CXX: " .. gxx)
    print("\t|-   CC : " .. gcc)

    os.execute("mkdir -p " .. BUILD_DIR)

    print("[4/6] Building libyumcxx.a...")
    local objs = {}
    for _, src in ipairs(SRC_CXX) do
        local obj = BUILD_DIR .. "/" .. src:match("([^/]+)%.cpp") .. ".o"
        run(string.format('%s -std=c++20 %s  -fdiagnostics-color=always -Wall -Wextra -fPIC -shared -c "%s" -o "%s"',
            CXX, make_includes(), src, obj))
        table.insert(objs, obj)
    end
    run(string.format('ar rcs "%s/libyumcxx.a" %s', BUILD_DIR, table.concat(objs, " ")))

    print("[5/6] Linking libyum.dylib...")
    local libs = join(LIBS, " ")
    run(string.format('%s -dynamiclib -o "%s" %s "%s/libyumcxx.a" %s -L"%s"  -fdiagnostics-color=always -shared -Wall -Wextra -DYUM_BUILD_DLL -fPIC',
        CXX, OUT_LIB, table.concat(SRC_C, " "), BUILD_DIR, make_includes(), LUA_LIB) .. " " .. libs)

    print("[6/6] Verifying...")
    run(CHECK_CMD .. " " .. OUT_LIB)

    print("Build complete: " .. OUT_LIB)
end

-- parse args (moved down so it can see globals)
function parse_args()
    local kind = "patch"
    for _, a in ipairs(arg) do
        if a == "-major" then kind = "major"
        elseif a == "-minor" then kind = "minor"
        elseif a:match("^%-studio:") then studio_name = a:sub(9)
        elseif a:match("^%-branch:") then branch_name = a:sub(9)
        elseif a:match("^%-cxx:") then CXX = a:sub(6)
        elseif a:match("^%-cc:") then CC = a:sub(5)
        elseif a:match("^%-lua_inc:") then LUA_INC = a:sub(10)
        elseif a:match("^%-lua_lib:") then LUA_LIB = a:sub(10)
        end
    end
    return kind
end

main()
