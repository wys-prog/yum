#!/bin/bash
set -e

# ----------------------
# Config
# ----------------------
CXX=g++-15
CC=gcc-15

LUA_INC="/opt/homebrew/include/lua5.4"
LUA_LIB="/opt/homebrew/lib"

SRC_CXX="src/lua/cxx/yuxx.cpp"
SRC_C="src/lua/yum.c"

BUILD_DIR="tests/bin"
OUT_LIB="$BUILD_DIR/../yum.dylib"

mkdir -p "$BUILD_DIR"

# ----------------------
# Step 1: build C++ core into static archive
# ----------------------
echo "[1/3] Building libyumcxx.a..."
$CXX -std=c++20 -I"$LUA_INC" -Wall -Wextra -fPIC -shared -c "$SRC_CXX" -o "$BUILD_DIR/yuxx.o"
ar rcs "$BUILD_DIR/libyumcxx.a" "$BUILD_DIR/yuxx.o"

# ----------------------
# Step 2: build final shared lib with static archive
# ----------------------
echo "[2/3] Linking libyum.dylib..."
$CXX -dynamiclib -o "$OUT_LIB" "$SRC_C" "$BUILD_DIR/libyumcxx.a" \
    -I"$LUA_INC" -L"$LUA_LIB" -shared -llua -lm -ldl \
    -Wall -Wextra -DYUM_BUILD_DLL -fPIC

# ----------------------
# Step 3: verify output
# ----------------------
echo "[3/3] Verifying..."
otool -L "$OUT_LIB"

echo "Build complete: $OUT_LIB"
