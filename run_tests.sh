#!/bin/bash

set -e

if [ "$#" -eq 1 ] && [ "$1" = "all" ]; then
    for compiler in gcc clang; do
        for mode in Debug Release; do
            $0 $compiler $mode
        done
    done
    exit 0
fi

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

# 1.5 Check for evil functions
check_evil() {
    evil="$1"
    replacements="$2"
    files="$(ls *.h)"
    for file in $files; do
        if [[ "$file" = "jc_util.h" && "$evil" = "strlen" ]]; then
            continue
        fi
        if grep -q "$evil" $file; then
            echo "Found 'evil' function $evil in $file"
            echo "Please replace with one of the following:"
            echo "$replacements"
            exit 1
        fi
    done
}

check_evil strlen strnlen_s
check_evil strcpy strncpy_s
check_evil strcat strncat_s
check_evil sprintf snprintf
check_evil vsprintf vsnprintf
check_evil scanf sscanf_s
check_evil sscanf sscanf_s
check_evil gets fgets

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
