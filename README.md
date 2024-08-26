<div align="center">
<pre>
░░░░░██╗██╗░░░░░██╗██████╗░░█████╗░
░░░░░██║██║░░░░░██║██╔══██╗██╔══██╗
░░░░░██║██║░░░░░██║██████╦╝██║░░╚═╝
██╗░░██║██║░░░░░██║██╔══██╗██║░░██╗
╚█████╔╝███████╗██║██████╦╝╚█████╔╝
░╚════╝░╚══════╝╚═╝╚═════╝░░╚════╝░
</pre>

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Tests](https://github.com/Jonas-205/jlibc/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/Jonas-205/jlibc/actions/workflows/cmake-multi-platform.yml)

</div>

A collection of C libraries that I use on a semi-regular basis.
I mostly try to follow the
[stb-style](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt) for these libraries,
which means each library is contained in a single header file.
You can easily add this file to a project or build system,
and it should just work.

I use Linux (specifically Ubuntu and Arch btw) for work, personal use and uni.
Because of this most of the testing and time is put into making it work smoothly on Linux.
Most features should also work on Windows and MacOS,
but they are only tested through the automated tests on Github actions.

## Usage
This is a collection of [stb-style](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt) header-file libraries,
simply copy the relevant header into your project and use whatever build system you want.

Do `#define LIBRARY_NAME_IMPLEMENTATION` before you include the file in *one* C or C++ file
to create the implementation.

```C
// i.e. it should look like this:
#include ...
#include ...
#include ...
#define JC_LOG_IMPLEMENTATION  // only in one C/C++ file
#include "jc_log.h"
```

## Examples
The examples directory contains a list of examples on how to use features of the library.
The included Makefile can be used to build them, but `gcc -o example example.c` should also work.

## Tests
The tests directory contains a number of test programs.
They are build using [CMake](https://cmake.org/) so that they can be run on any OS.

## Libraries
See each specific header for documentation, also take a look at the provided examples.

Library | Short description
-|-
[jc_log](https://github.com/Jonas-205/jlibc/blob/main/docs/jc_log.md) | Logging macros for fancy printing and easy control over log files, log levels.
[jc_util](https://github.com/Jonas-205/jlibc/blob/main/docs/jc_util.md) | Utility functions for common tasks, like parsing numbers, reading lines, etc.
[jc_memory](https://github.com/Jonas-205/jlibc/blob/main/docs/jc_memory.md) | Memory safety helpers. Detecting missing frees and memory corruptions
