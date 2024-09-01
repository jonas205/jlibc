#include "memory.h"

static int callbacks = 0;

void callback(JcMemoryError *error) {
    callbacks += 1;
    switch (error->kind) {
        case JcMemoryErrorType_NotFreed:
            jcli__log(error->file, error->line, JCL_LEVEL_ERROR, true,
                      "%lu Bytes allocated here but not freed", error->size);
            if (error->line == 42) {
                return;
            }
        break;
        case JcMemoryErrorType_UnknownPtr:
            jcli__log(error->file, error->line, JCL_LEVEL_ERROR, true,
                      "Tried to free unknown Pointer here");
            if (error->line == 47) {
                return;
            }
        break;
        case JcMemoryErrorType_MemoryCorruption:
            jcli__log(error->file, error->line, JCL_LEVEL_ERROR, true,
                      "Memory Corruption detected here. Expected: '%x' Got: '%x' Offset: %ld",
                      error->memory_corruption.expected,
                      error->memory_corruption.gotten,
                      error->memory_corruption.offset);
        break;
    }
    jcl_assert(false, "Nothing bad should happen in this test");
}

int main(void) {
    jcm_create(&callback);

    char *foo = (char *) realloc(NULL, 10);
    free(foo);

    char *bar = (char *) realloc(NULL, 0);
    free(bar);

    char *baz = (char *) realloc(NULL, 10);
    (void) baz;

    #ifndef JC_MEMORY_DISABLE
    char fooo[10];
    char *fooooo = realloc(fooo, 0);  // Calls callback
    (void) fooooo;
    #endif

    char *foooo = realloc(NULL, 0);
    free(foooo);

    char *a = (char *) realloc(NULL, 10);

    for (int i = 0; i < 10; i++) {
        a[i] = i;
    }

    a = realloc(a, 20);

    for (int i = 10; i < 20; i++) {
        a[i] = i;
    }

    for (int i = 0; i < 20; i++) {
        jcl_assert(a[i] == i, "Realloc failed");
    }

    a = realloc(a, 0);

    jcm_destroy();
    #ifndef JC_MEMORY_DISABLE
    jcl_assert(callbacks == 2, "Callback not called");
    #endif
}

