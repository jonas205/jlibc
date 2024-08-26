#include "memory.h"

int main(void) {
    jcm_create(&callback);

    const char *str = (char*) malloc(42);  // Not Freed
    (void) str;

    jcm_destroy();
}

