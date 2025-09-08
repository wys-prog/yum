---@diagnostic disable: need-check-nil

-- ========================
-- CONFIGURATION (easy to edit)
-- ========================
local studio_name  = "yum_official" -- studio building this
local branch_name  = "main"        -- git branch or any string
local CXX          = "g++-15"      -- C++ compiler
local CC           = "gcc-15"      -- C compiler
local LUA_INC      = "/opt/homebrew/include/lua5.4"
local LUA_LIB      = "/opt/homebrew/lib"
local BUILD_DIR    = "tests/bin"
local OUT_LIB      = BUILD_DIR .. "/../yum"  -- we'll append .dylib or .dll
local CHECK_CMD    = "otool -L"
local version_file = "src/lua/yumv.h"

local SRC_CXX = {
  "src/lua/cxx/yuxx.cpp",
  "src/lua/cxx/variant.cpp",
  "src/lua/cxx/vec.cpp",
}
local SRC_C = {
  "src/lua/yum.c",
}
local INCLUDES = {
  LUA_INC,
  "src/lua/cxx",
}
local LIBS = {
  "-llua", "-lm", "-ldl"
}

-- helpers
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

local function run(cmd)
  print("\t> " .. cmd)
  local handle = io.popen(cmd .. " 2>&1")
  local result = handle:read("*a")
  local success, _, code = handle:close()
  if code ~= 0 then
    error("Command failed with code " .. code .. "\nOutput:\n" .. result)
  end
end

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

local function parse_args()
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

-- ========================
-- BUILD STEPS
-- ========================
local function generate_build_steps()
  local steps = {}
  table.insert(steps, 'mkdir -p ' .. BUILD_DIR)

  local src_files = join(SRC_C, " ") .. " " .. join(SRC_CXX, " ")
  local includes = make_includes()
  local libs     = join(LIBS, " ")

  -- macOS dylib
  local dylib = OUT_LIB .. ".dylib"
  table.insert(steps, string.format(
    '%s -dynamiclib -o "%s" %s %s -L"%s" %s -std=c++20 -fPIC',
    CXX, dylib, src_files, includes, LUA_LIB, libs
  ))
  table.insert(steps, CHECK_CMD .. " " .. dylib)

  -- Windows DLL with MinGW
  local dll = OUT_LIB .. ".dll"
  table.insert(steps, string.format(
    'x86_64-w64-mingw32-g++ -shared -o "%s" %s %s -L"%s" %s -std=c++20 -static-libgcc -static-libstdc++',
    dll, src_files, includes, LUA_LIB, libs
  ))

  return steps
end


-- ========================
-- MAIN
-- ========================
local function main()
  print("==== Yum Build System ====")
  print("[1/6] Parsing args...")
  local kind = parse_args()

  print("[2/6] Bumping version...")
  local ver = bump_version(kind)
  print("\tVersion: " .. ver)
  print("\tStudio: " .. studio_name)
  print("\tBranch: " .. branch_name)

  local build_steps = generate_build_steps()
  for i, cmd in ipairs(build_steps) do
    print(string.format('[%d/%d] %s', i, #build_steps, cmd))
    run(cmd)
  end

  print("Build complete!")
end

main()
