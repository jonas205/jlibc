#include <signal.h>
#include <stdlib.h>
#define JC_LOG_IMPLEMENTATION
#include "../../jc_log.h"

void sigill_handler(int sig) {
    (void)sig;
    printf("SIGILL\n");
    exit(0);
}

int main(void) {
#ifndef JC_LOG_WINDOWS  // Windows uses debug break
#ifndef JC_LOG_APPLE  // Die works on apple, but causes SIGTRAP
    signal(SIGILL, sigill_handler);
    die("DIE");
    return -1;
#endif
#endif
}
