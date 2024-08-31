/* jc_memory - v0.0.1

Do this:
    #define JC_MEMORY_IMPLEMENTATION
before you include this file in *one* C or C++ file to create the implementation.

// i.e. it should look like this:
#include ...
#include ...
#include ...
#define JC_MEMORY_IMPLEMENTATION
#include "jc_memory.h"

QUICK NOTES:
    - I mostly try to follow the stb-style in this library
        (https://github.com/nothings/stb/blob/master/docs/stb_howto.txt)
    - This library wraps malloc (free, calloc, realloc and reallocarray),
        include it before any other header that might use them

THINGS YOU CAN DEFINE:
    DEFINE EVERYWHERE:
    - JC_MEMORY_STATIC                  Make all functions static, only use this if you know what you are doing
    - JC_MEMORY_DISABLE                 Disables all logging (die still works and prints only the message to stderr)

    DEFINE IN SAME FILE AS JC_MEMORY_IMPLEMENTATION:
    - JC_MEMORY_CANARY <uint32_t>       The Canary Bytes to be used
    - JC_MEMORY_CANARY_REPETITION <int> How often the Canary Bytes should be added on both size

LICENSE:
    See end of file for license information
*/


#ifndef JC_MEMORY_H_
#define JC_MEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(WIN32)
#define JC_MEMORY_WINDOWS
elif __APPLE__
#define JC_MEMORY_APPLE
#endif

#ifndef JC_MEMORY_DEF
#ifdef JC_MEMORY_STATIC
#define JC_MEMORY_DEF static
#else
#define JC_MEMORY_DEF extern
#endif
#endif

#ifdef JC_MEMORY_WINDOWS
#define jcm_force_inline static __forceinline
#else
#define jcm_force_inline static __attribute__((always_inline)) inline
#endif

// TODO(me): Once C23 comes out, check for it and use its feature if possible
#ifndef jcm_warn_unused
#ifdef JC_MEMORY_WINDOWS  // afaik windows does not support checking for unused results
#define jcm_warn_unused
#else
#define jcm_warn_unused __attribute__((warn_unused_result))
#endif
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

typedef enum {
    JcMemoryErrorType_NotFreed,  // jcm_destroy called while not everything was freed
    JcMemoryErrorType_UnknownPtr,  // e.g. calling realloc with a non malloced non NULL pointer
    JcMemoryErrorType_MemoryCorruption,  // Bytes around the allocation changed
} JcMemoryErrorType;

typedef union {
    struct {
        JcMemoryErrorType kind;
        const char *file;  // Where the ptr was created (malloc / calloc / realloc)
        uint32_t line;  // Where the ptr was created (malloc / calloc / realloc)
        size_t size;  // The size of the pointer
    };  // Shared across all errors
    struct {
        JcMemoryErrorType kind;
        const char *file;
        uint32_t line;
        size_t size;
        uint32_t expected;  // The bytes that should be there
        uint32_t gotten;  // The bytes that are there
        // Location of the wrongfully written bytes.
        // Negative offset: To the left of the allocation
        // Positive offset: To the right of the allocation
        int64_t offset;
    } memory_corruption;
} JcMemoryError;

typedef void (*JcMemoryErrorCallback)(JcMemoryError *error);

#ifndef JC_MEMORY_DISABLE
JC_MEMORY_DEF void jcm_create(JcMemoryErrorCallback callback);
JC_MEMORY_DEF void jcm_destroy(void);
#else
#define jcm_create(callback)
#define jcm_destroy()
#endif

#ifndef JC_MEMORY_DISABLE
JC_MEMORY_DEF void jcm_warn_unused *jcmi__malloc(size_t size, const char *file, uint32_t line);
JC_MEMORY_DEF void jcmi__free(void *ptr, const char *file, uint32_t line);
JC_MEMORY_DEF void jcm_warn_unused *jcmi__calloc(size_t nmemb, size_t size, const char *file, uint32_t line);
JC_MEMORY_DEF void jcm_warn_unused *jcmi__realloc(void *ptr, size_t size, const char *file, uint32_t line);

#ifndef JC_MEMORY_IMPLEMENTATION
#define malloc(size) jcmi__malloc(size, __FILE__, __LINE__)
#define free(ptr) jcmi__free(ptr, __FILE__, __LINE__)
#define calloc(nmemb, size) jcmi__calloc(nmemb, size, __FILE__, __LINE__)
#define realloc(ptr, size) jcmi__realloc(ptr, size, __FILE__, __LINE__)
#endif
#endif

#ifndef JC_MEMORY_DISABLE
#ifndef JC_MEMORY_DISABLE_CANARY
JC_MEMORY_DEF void jcmi__check_canary(void *ptr, const char *file, uint32_t line);
#define jcm_check_canary(ptr) jcmi__check_canary(ptr, __FILE__, __LINE__)
#else
#define jcm_check_canary(ptr)
#endif
#else
#define jcm_check_canary(ptr)
#endif

#ifdef __cplusplus
}
#endif
//
//
////   end header file   /////////////////////////////////////////////////////
#endif  // JC_MEMORY_H_

#ifdef JC_MEMORY_IMPLEMENTATION
#ifdef __cplusplus
extern "C" {
#endif

#ifndef JC_MEMORY_DISABLE
#include <stdbool.h>

#ifndef JC_MEMORY_CANARY
#define JC_MEMORY_CANARY 0xdeadbeef
#endif
#define JC_MEMORY_CANARY_SIZE 4
#ifndef JC_MEMORY_CANARY_REPETITION
#define JC_MEMORY_CANARY_REPETITION 1024
#endif


static JcMemoryErrorCallback JCI__MEMORY_CALLBACK = NULL;

typedef struct {
    void *ptr;
    size_t size;
    const char *file;
    uint32_t line;
} Jci__MemoryAllocation;

static struct {
    Jci__MemoryAllocation *allocations;
    size_t count;
    size_t capacity;
} JCI__MEMORY_ALLOCATION_LIST;

JC_MEMORY_DEF void jcm_create(JcMemoryErrorCallback callback) {
    JCI__MEMORY_CALLBACK = callback;

    JCI__MEMORY_ALLOCATION_LIST.allocations = NULL;
    JCI__MEMORY_ALLOCATION_LIST.count = 0;
    JCI__MEMORY_ALLOCATION_LIST.capacity = 0;
}

JC_MEMORY_DEF void jcm_destroy(void) {
    for (size_t i = 0; i < JCI__MEMORY_ALLOCATION_LIST.count; i++) {
        JcMemoryError error;
        Jci__MemoryAllocation allocation = JCI__MEMORY_ALLOCATION_LIST.allocations[i];

        error.kind = JcMemoryErrorType_NotFreed;
        error.file = allocation.file;
        error.line = allocation.line;
        #ifndef JC_MEMORY_DISABLE_CANARY
        error.size = allocation.size - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION * 2;
        #else
        error.size = allocation.size;
        #endif

        JCI__MEMORY_CALLBACK(&error);
    }
    free(JCI__MEMORY_ALLOCATION_LIST.allocations);
    JCI__MEMORY_ALLOCATION_LIST.allocations = NULL;
    JCI__MEMORY_ALLOCATION_LIST.count = 0;
    JCI__MEMORY_ALLOCATION_LIST.capacity = 0;
}

static void jcmi__allocation_add(void *ptr, size_t size, const char *file, uint32_t line) {
    if (JCI__MEMORY_ALLOCATION_LIST.count == JCI__MEMORY_ALLOCATION_LIST.capacity) {
        JCI__MEMORY_ALLOCATION_LIST.capacity =
            JCI__MEMORY_ALLOCATION_LIST.capacity == 0 ? 8 : JCI__MEMORY_ALLOCATION_LIST.capacity * 2;
        JCI__MEMORY_ALLOCATION_LIST.allocations = (Jci__MemoryAllocation *)
            realloc(JCI__MEMORY_ALLOCATION_LIST.allocations,
                    JCI__MEMORY_ALLOCATION_LIST.capacity * sizeof(Jci__MemoryAllocation));
    }
    Jci__MemoryAllocation *allocation =
        &JCI__MEMORY_ALLOCATION_LIST.allocations[JCI__MEMORY_ALLOCATION_LIST.count++];
    allocation->ptr = ptr;
    allocation->size = size;
    allocation->file = file;
    allocation->line = line;
}

static bool jcmi__allocation_remove(void *ptr, size_t *size, const char **file, uint32_t *line) {
    for (size_t i = 0; i < JCI__MEMORY_ALLOCATION_LIST.count; i++) {
        Jci__MemoryAllocation *allocation = &JCI__MEMORY_ALLOCATION_LIST.allocations[i];
        if (allocation->ptr != ptr) {
            continue;
        }
        *size = allocation->size;
        *file = allocation->file;
        *line = allocation->line;

        JCI__MEMORY_ALLOCATION_LIST.count--;
        JCI__MEMORY_ALLOCATION_LIST.allocations[i] =
            JCI__MEMORY_ALLOCATION_LIST.allocations[JCI__MEMORY_ALLOCATION_LIST.count];

        return true;
    }
    return false;
}

#ifndef JC_MEMORY_DISABLE_CANARY
static bool jcmi__allocation_get(void *ptr, size_t *size, const char **file, uint32_t *line) {
    for (size_t i = 0; i < JCI__MEMORY_ALLOCATION_LIST.count; i++) {
        Jci__MemoryAllocation *allocation = &JCI__MEMORY_ALLOCATION_LIST.allocations[i];
        if (allocation->ptr != ptr) {
            continue;
        }
        *size = allocation->size;
        *file = allocation->file;
        *line = allocation->line;

        return true;
    }
    return false;
}
#endif

JC_MEMORY_DEF void jcm_warn_unused *jcmi__malloc(size_t size, const char *file, uint32_t line) {
    #ifndef JC_MEMORY_DISABLE_CANARY
    size += JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION * 2;
    #endif

    void *ptr = malloc(size);
    jcmi__allocation_add(ptr, size, file, line);

    #ifndef JC_MEMORY_DISABLE_CANARY
    uint32_t *canary_left = (uint32_t *) ptr;
    uint32_t *canary_right = (uint32_t *)
        (((uint8_t *) ptr) + size - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION);
    for (size_t i = 0; i < JC_MEMORY_CANARY_REPETITION; i++) {
        *canary_left++ = JC_MEMORY_CANARY;
        *canary_right++ = JC_MEMORY_CANARY;
    }

    ptr = ((uint8_t *) ptr) + JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION;
    #endif

    return ptr;
}

JC_MEMORY_DEF void jcmi__free(void *ptr, const char *file, uint32_t line) {
    if (!ptr) {
        return;
    }

    #ifndef JC_MEMORY_DISABLE_CANARY
    ptr = ((uint8_t *) ptr) - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION;
    #endif

    size_t old_size;
    const char *old_file;
    uint32_t old_line;

    bool found = jcmi__allocation_remove(ptr, &old_size, &old_file, &old_line);

    if (!found) {
        JcMemoryError error;

        error.kind = JcMemoryErrorType_UnknownPtr;
        error.file = file;
        error.line = line;
        error.size = 0;

        JCI__MEMORY_CALLBACK(&error);
        return;
    }

    #ifndef JC_MEMORY_DISABLE_CANARY
    uint32_t *canary_left = (uint32_t *) ptr;
    uint32_t *canary_right = (uint32_t *)
        (((uint8_t *) ptr) + old_size - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION);
    for (size_t i = 0; i < JC_MEMORY_CANARY_REPETITION; i++) {
        if (*canary_left != JC_MEMORY_CANARY) {
            JcMemoryError error;

            error.kind = JcMemoryErrorType_MemoryCorruption;
            error.file = file;
            error.line = line;
            error.size = old_size - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION * 2;
            error.memory_corruption.expected = JC_MEMORY_CANARY;
            error.memory_corruption.gotten = *canary_left;
            error.memory_corruption.offset =
                (-JC_MEMORY_CANARY_REPETITION + i) * JC_MEMORY_CANARY_SIZE;

            JCI__MEMORY_CALLBACK(&error);
        }
        if (*canary_right != JC_MEMORY_CANARY) {
            JcMemoryError error;

            error.kind = JcMemoryErrorType_MemoryCorruption;
            error.file = file;
            error.line = line;
            error.size = old_size - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION * 2;
            error.memory_corruption.expected = JC_MEMORY_CANARY;
            error.memory_corruption.gotten = *canary_right;
            error.memory_corruption.offset = i * JC_MEMORY_CANARY_SIZE;

            JCI__MEMORY_CALLBACK(&error);
        }
        canary_left++;
        canary_right++;
    }
    #endif

    free(ptr);
}

JC_MEMORY_DEF void jcm_warn_unused *jcmi__calloc(size_t nmemb, size_t size, const char *file, uint32_t line) {
    if (size != 0 && nmemb > SIZE_MAX / size) {
        return NULL;
    }
    return jcmi__malloc(nmemb * size, file, line);
}

JC_MEMORY_DEF void jcm_warn_unused *jcmi__realloc(void *ptr, size_t size, const char *file, uint32_t line) {
    if (!ptr) {
        return jcmi__malloc(size, file, line);
    }
    if (size == 0) {
        jcmi__free(ptr, file, line);
        return NULL;
    }

    #ifndef JC_MEMORY_DISABLE_CANARY
    ptr = ((uint8_t *) ptr) - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION;
    size += JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION * 2;
    #endif

    size_t old_size;
    const char *old_file;
    uint32_t old_line;

    bool found = jcmi__allocation_remove(ptr, &old_size, &old_file, &old_line);
    if (!found) {
        JcMemoryError error;

        error.kind = JcMemoryErrorType_UnknownPtr;
        error.file = file;
        error.line = line;
        #ifndef JC_MEMORY_DSABLE_CANARY
        error.size = size - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION * 2;
        #else
        error.size = size;
        #endif

        JCI__MEMORY_CALLBACK(&error);
        return NULL;
    }

    ptr = realloc(ptr, size);
    jcmi__allocation_add(ptr, size, file, line);

    #ifndef JC_MEMORY_DISABLE_CANARY
    uint32_t *canary_left = (uint32_t *) ptr;
    uint32_t *canary_right = (uint32_t *)
        (((uint8_t *) ptr) + old_size - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION);
    for (size_t i = 0; i < JC_MEMORY_CANARY_REPETITION; i++) {
        if (*canary_left != JC_MEMORY_CANARY) {
            JcMemoryError error;

            error.kind = JcMemoryErrorType_MemoryCorruption;
            error.file = file;
            error.line = line;
            error.size = old_size - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION * 2;
            error.memory_corruption.expected = JC_MEMORY_CANARY;
            error.memory_corruption.gotten = *canary_left;
            error.memory_corruption.offset =
                (-JC_MEMORY_CANARY_REPETITION + i) * JC_MEMORY_CANARY_SIZE;

            JCI__MEMORY_CALLBACK(&error);
        }
        if (*canary_right != JC_MEMORY_CANARY) {
            JcMemoryError error;

            error.kind = JcMemoryErrorType_MemoryCorruption;
            error.file = file;
            error.line = line;
            error.size = old_size - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION * 2;
            error.memory_corruption.expected = JC_MEMORY_CANARY;
            error.memory_corruption.gotten = *canary_right;
            error.memory_corruption.offset = i * JC_MEMORY_CANARY_SIZE;

            JCI__MEMORY_CALLBACK(&error);
        }
        canary_left++;
        canary_right++;
    }

    uint32_t *new_canary_left = (uint32_t *) ptr;
    uint32_t *new_canary_right = (uint32_t *)
        (((uint8_t *) ptr) + size - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION);

    for (size_t i = 0; i < JC_MEMORY_CANARY_REPETITION; i++) {
        *new_canary_left++ = JC_MEMORY_CANARY;
        *new_canary_right++ = JC_MEMORY_CANARY;
    }

    ptr = ((uint8_t *) ptr) + JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION;

    #endif

    return ptr;
}

#ifndef JC_MEMORY_DISABLE_CANARY
JC_MEMORY_DEF void jcmi__check_canary(void *ptr, const char *file, uint32_t line) {
    ptr = ((uint8_t *) ptr) - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION;


    size_t old_size;
    const char *old_file;
    uint32_t old_line;

    bool found = jcmi__allocation_get(ptr, &old_size, &old_file, &old_line);
    if (!found) {
        JcMemoryError error;

        error.kind = JcMemoryErrorType_UnknownPtr;
        error.file = file;
        error.line = line;
        error.size = 0;

        JCI__MEMORY_CALLBACK(&error);
    }

    uint32_t *canary_left = (uint32_t *) ptr;
    uint32_t *canary_right = (uint32_t *)
        (((uint8_t *) ptr) + old_size - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION);
    for (size_t i = 0; i < JC_MEMORY_CANARY_REPETITION; i++) {
        if (*canary_left != JC_MEMORY_CANARY) {
            JcMemoryError error;

            error.kind = JcMemoryErrorType_MemoryCorruption;
            error.file = file;
            error.line = line;
            error.size = old_size - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION * 2;
            error.memory_corruption.expected = JC_MEMORY_CANARY;
            error.memory_corruption.gotten = *canary_left;
            error.memory_corruption.offset =
                (-JC_MEMORY_CANARY_REPETITION + i) * JC_MEMORY_CANARY_SIZE;

            JCI__MEMORY_CALLBACK(&error);
        }
        if (*canary_right != JC_MEMORY_CANARY) {
            JcMemoryError error;

            error.kind = JcMemoryErrorType_MemoryCorruption;
            error.file = file;
            error.line = line;
            error.size = old_size - JC_MEMORY_CANARY_SIZE * JC_MEMORY_CANARY_REPETITION * 2;
            error.memory_corruption.expected = JC_MEMORY_CANARY;
            error.memory_corruption.gotten = *canary_right;
            error.memory_corruption.offset = i * JC_MEMORY_CANARY_SIZE;

            JCI__MEMORY_CALLBACK(&error);
        }
        canary_left++;
        canary_right++;
    }
}
#endif

#define malloc(size) jcmi__malloc(size, __FILE__, __LINE__)
#define free(ptr) jcmi__free(ptr, __FILE__, __LINE__)
#define calloc(nmemb, size) jcmi__calloc(nmemb, size, __FILE__, __LINE__)
#define realloc(ptr, size) jcmi__realloc(ptr, size, __FILE__, __LINE__)
#endif

#ifdef __cplusplus
}
#endif
#endif  // JC_MEMORY_IMPLEMENTATION

/*
------------------------------------------------------------------------------
MIT License

Copyright (c) 2024 Jonas Kristen

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
