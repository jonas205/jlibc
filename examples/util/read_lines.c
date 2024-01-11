#define JC_UTIL_IMPLEMENTATION
#include "../../jc_util.h"

void reader(const char *line, ssize_t len) {
    printf("Len: %.2ld, Content: %s", len, line);
}

int main(void) {
    return !jcu_line_parser("read_lines.c", reader);
}
