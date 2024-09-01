#include "memory.h"

int main(void) {
    jcm_create(&callback);

    char *hello = "Hello World!";
    free(hello);

    jcm_destroy();
}

