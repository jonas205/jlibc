#define JC_UTIL_IMPLEMENTATION
#include "../../jc_util.h"

void reader(const char *line, ssize_t len) {
    char buf[512];
    snprintf(buf, sizeof(buf), "%s", line);
    buf[len] = '\0';

    printf("Len: %.2ld, Content: %s\n", len, buf);
}

int main(void) {
    return !jcu_line_parser("read_lines.c", reader);
}
