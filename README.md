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

</div>

A collection of C libraries that I use on a semi-regular basis.
I mostly try to follow the
[stb-style](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt) for these libraries,
which means each library is contained in a single header file.
You can easily add this file to a project or build system,
and it should just work.

I use Linux (Ubuntu and Arch btw) for work / personal / uni,
so most of the testing / time is put into making it work on Linux.
Most features should also work on Windows / MacOS,
but they are only tested through the automated tests on Github actions.

## Usage
This is a [stb-style](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt) header-file library,
so simply copy the relevant header into your project and use whatever build system you want.

Do `#define LIBRARY_NAME_IMPLEMENTATION` before you include this file in *one* C or C++ file
to create the implementation.

```C
// i.e. it should look like this:
#include ...
#include ...
#include ...
#define JLOG_IMPLEMENTATION  // only in one C/C++ file
#include "jlog.h"
```

## Examples
The examples directory contains a list of examples on how to use features of the library.
The included Makefile can be used to build them, but `gcc -o example example.c` should also work.

## Tests
The tests directory contains a number of tests programs.
They are build using cmake so that they can be run on any OS.

## Libraries
See each specific header for documentation, also take a look at the provided examples.

Library | Short description
-|-
`jlog.h` | Logging macros for fancy printing / easy control over log files, log levels.
