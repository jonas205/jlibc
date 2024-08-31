#include "memory.h"

static char *missing_free = 0;
static int callbacks = 0;

void callback(JcMemoryError *error) {
    callbacks += 1;
    switch (error->kind) {
        case JcMemoryErrorType_NotFreed:
            jcli__log(error->file, error->line, JCL_LEVEL_ERROR, true,
                      "%lu Bytes allocated here but not freed", error->size);

            if (missing_free != 0) {
                jcl_assert(error->line == 46, "Not the right malloc detected");
                missing_free = 0;
                return;
            }
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
        break;
    }
    jcl_assert(false, "Nothing bad should happen in this test");
}

int main(void) {
    jcm_create(&callback);

    char *foo = (char *) malloc(10);
    free(foo);

    char *bar = (char *) malloc(0);
    free(bar);

    jcm_destroy();
    jcm_create(&callback);

    char *baz = (char *) malloc(10);
    missing_free = baz;

    jcm_destroy();

    jcl_assert(callbacks == 1, "Callback not called");
}

