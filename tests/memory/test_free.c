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
            jcl_assert(error->line == 37, "Not the correct wrong free detected");
            return;
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

    #ifndef JC_MEMORY_DISABLE
    char bar[10];
    free(bar);  // Calls callback
    #endif

    free(0);

    jcm_destroy();

    #ifndef JC_MEMORY_DISABLE
    jcl_assert(callbacks == 1, "Callback not called");
    #endif
}

