#include "memory.h"

int main(void) {
    jcm_create(&callback);

    const char *hello = "Hello World!";  // 12 + 1
    char *str = (char*) malloc(10);  // 10
    memcpy(str, hello, strlen(hello) + 1);  // Written 13 bytes to 10 byte buffer
    printf("%s\n", str);

    jcm_check_canary(str);  // Calls callback
    free(str);  // Calls callback

    jcm_destroy();
}

