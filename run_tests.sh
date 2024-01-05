#!/bin/bash

set -e

# 0. Check inputs

print_usage() {
    echo "Usage: $0 <compiler> <mode>"
    echo "  compiler: gcc, clang (passed to cmake. Other compilers may work, but are untested)"
    echo "  mode: Debug, Release"
}

if [ "$#" -ne 2 ]; then
    print_usage
    exit 1
fi

CC="$1"
MODE="$2"

if [ "$MODE" != "Debug" ] && [ "$MODE" != "Release" ]; then
    echo "Invalid mode: $MODE"
    print_usage
    exit 1
fi

# 1. Linter
echo " --- Running linter ---"
cpplint --exclude tests/build --recursive .

# 2. Make sure examples compile
echo " --- Building examples --- "
make -C examples clean
if [ "$MODE" = "Debug" ]; then
    make -C examples all CC=$CC EXTRA_FLAGS="-g -O0"
else
    make -C examples all CC=$CC EXTRA_FLAGS="-O3"
fi;

# 3. Run tests
echo " --- Running tests --- "

cd "$(dirname "$0")"/tests

mkdir -p build/$CC/$MODE
cd build/$CC/$MODE
cmake -DCMAKE_BUILD_TYPE=$MODE -DCMAKE_C_COMPILER=$CC ../../..
make
ctest --output-on-failure
