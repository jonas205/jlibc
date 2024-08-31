#include "memory.h"

static int callbacks = 0;

void callback(JcMemoryError *error) {
    callbacks += 1;
    switch (error->kind) {
        case JcMemoryErrorType_NotFreed:
            jcli__log(error->file, error->line, JCL_LEVEL_ERROR, true,
                      "%lu Bytes allocated here but not freed", error->size);
        break;
        case JcMemoryErrorType_UnknownPtr:
            jcli__log(error->file, error->line, JCL_LEVEL_ERROR, true,
                      "Tried to free unknown Pointer here");
        break;
        case JcMemoryErrorType_MemoryCorruption:
            jcli__log(error->file, error->line, JCL_LEVEL_ERROR, true,
                      "Memory Corruption detected here. Expected: '%x' Got: '%x' Offset: %ld",
                      error->memory_corruption.expected,
                      error->memory_corruption.gotten,
                      error->memory_corruption.offset);
            jcl_assert(error->line == 42 ||
                       error-> line == 48 ||
                       error-> line == 54 ||
                       error-> line == 58
                       , "Corruption detected wrong");
            return;
        break;
    }
    jcl_assert(false, "Nothing bad should happen in this test");
}

int main(void) {
    jcm_create(&callback);

    char *foo;

    foo = (char *) malloc(10);
    for (int i = 0; i < 20; i++) {
        foo[i] = i;
    }
    free(foo);  // Calls callback 3 times

    foo = (char *) malloc(10);
    for (int i = -10; i < 10; i++) {
        foo[i] = i;
    }
    free(foo);  // Calls callback 3 times

    foo = (char *) malloc(10);
    for (int i = -10; i < 20; i++) {
        foo[i] = i;
    }
    foo = realloc(foo, 20);
    for (int i = -10; i < 30; i++) {
        foo[i] = i;
    }
    free(foo);



    jcm_destroy();
    jcl_assert(callbacks == 18, "Callbacks missing");
}


