# Documentation [jc_memory.h](../jc_memory.h)


## TODO
- Implement JC_MEMORY_DISABLE
- Use a better data structure

jc_memory.h is a
[stb-style](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt)
header only library.
This means which means the library is contained in a single header file.
You can easily add the header file to a project or build system,
without having to worry about what build other build systems.

## Basic Usage
```C
#define JC_MEMORY_IMPLEMENTATION   // Only do this in one C / C++ file
#include "jc_memory.h"

void callback(JcMemoryError *error) {
    ...
}

int main(void) {
    jcm_create(&callback);  // Call once at program start

    const char *str_1 = (char*) malloc(42);  // Not Freed, caught by jcm_destroy()
    (void) str;

    const char *str_2 = (char*) malloc(10);
    str_2[50] = 'a';  // Illegal memory access
    free();  // Catches access and calls callback

    jcm_destroy();  // Call once at program end
}
```

## Prefix
All functions / macros included in this library have the prefix `jcm_`
to avoid name conflicts with other libraries.

## Memory Checks
The main feature of the library is to wrap functions calls to
`malloc`, `free`, `realloc` and `calloc` to achieve extra memory safety.
By wrapping these functions the library can warn if any illegal
memory access happens, or can show memory allocations that where never freed.

## Memory Canary
Memory Corruptions like, in the examples, are caught in `free` or `realloc`.
Importantly not all memory corruptions can be caught by this library,
e.g. if a 1 KiB Canary is placed on the left on the right of the allocation,
but a corruption with an offset of 2 KiB takes place, it is not caught.

When one wants to manually check for corruptions one can call
`jcm_check_canary(ptr)`.

## Error Callback
To make sure the behavior of the wrapped functions is not changed,
the callback functions will be called when an error is detected.
The user (you) then has to decide what to do

```C
void callback(JcMemoryError *error) {
    switch (error->kind) {
        case JcMemoryErrorType_NotFreed:  // Called on jcm_destroy
            error->file;  // Where was the memory allocated
            error->line;  // Where was the memory allocated
            error->size;  // Size of the allocation (without canary bytes)
            // This does not free the memory
            // Do not free it here, fix your bug :)
        break;
        case JcMemoryErrorType_UnknownPtr:
            error->file;  // Where the error occurred
            error->line;  // Where the error occurred
            error->size;  // 0
            // free or realloc was passed a pointer that was not allocated by this library
        break;
        case JcMemoryErrorType_MemoryCorruption:
            error->file;  // Where the error detected
            error->line;  // Where the error detected
            error->size;  // Size of the allocation (without canary bytes)
            error->memory_corruption.expected;  // Expected Bytes
            error->memory_corruption.gotten;    // Found Bytes
            // Offset from the allocation where the corruption occurred
            // Positive offset means to the right
            // Negative offset means to the left
            error->memory_corruption.offset;
        break;
    }
}
```


## Compile Options
### JC_MEMORY_STATIC
Makes all functions static, only use this if you know what you are doing, as it can lead to larger programs otherwise.

### JC_MEMORY_DISABLE
Completely disable the library at compile time without changing any code. 
Define everywhere where you include it.


### JC_MEMORY_CANARY
Define the 4 Bytes that are used as Canary Bytes.
They will be repeated infinitely.

### JC_MEMORY_CANARY_REPETITIONS
How often the Canary Bytes should be added on both sides.
E.g. if 1024, the 4 Bytes defined by JC_MEMORY_CANARY will be repeated
1024 times on the left and 1024 on the right,
Making the allocation 8 KiB larger.

