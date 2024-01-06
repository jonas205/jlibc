#error Template Replacement Start
// Replace these:
// %s/\<jc_template/jc_lib/gI
// %s/\<JC_TEMPLATE/JC_LIB/gI
// %s/\<jct_/jcl_/gI
#error Template Replacement End

/* jc_template - v0.0.1

Do this:
    #define JC_TEMPLATE_IMPLEMENTATION
before you include this file in *one* C or C++ file to create the implementation.

// i.e. it should look like this:
#include ...
#include ...
#include ...
#define JC_TEMPLATE_IMPLEMENTATION
#include "jc_template.h"

QUICK NOTES:
    - I mostly try to follow the stb-style in this library
        (https://github.com/nothings/stb/blob/master/docs/stb_howto.txt)

THINGS YOU CAN DEFINE:
    DEFINE EVERYWHERE:

    DEFINE IN SAME FILE AS JC_TEMPLATE_IMPLEMENTATION:

LICENSE:
    See end of file for license information
*/


#ifndef JC_TEMPLATE_H_
#define JC_TEMPLATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(WIN32)
#define JC_TEMPLATE_WINDOWS
elif __APPLE__
#define JC_TEMPLATE_APPLE
#endif

#ifndef JC_TEMPLATE_DEF
#ifdef JC_TEMPLATE_STATIC
#define JC_TEMPLATE_DEF static
#else
#define JC_TEMPLATE_DEF extern
#endif
#endif

#ifdef JC_TEMPLATE_WINDOWS
#define jct_force_inline static __forceinline
#else
#define jct_force_inline static __attribute__((always_inline)) inline
#endif

// TODO(me): Once C23 comes out, check for it and use its feature if possible
#ifndef jct_warn_unused
#ifdef JC_TEMPLATE_WINDOWS  // afaik windows does not support checking for unused results
#define jct_warn_unused
#else
#define jct_warn_unused __attribute__((warn_unused_result))
#endif
#endif

#ifdef __cplusplus
}
#endif
//
//
////   end header file   /////////////////////////////////////////////////////
#endif  // JC_TEMPLATE_H_

#ifdef JC_TEMPLATE_IMPLEMENTATION
#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif
#endif  // JC_TEMPLATE_IMPLEMENTATION

/*
------------------------------------------------------------------------------
MIT License

Copyright (c) 2023 Jonas Kristen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
*/
