#include <stdlib.h>
#define JC_UTIL_IMPLEMENTATION
#include "../../jc_util.h"
#define JC_LOG_IMPLEMENTATION
#include "../../jc_log.h"

void reader(const char *line, ssize_t len) {
    (void) len;
    char buf[512];
    int64_t n = strtol(line, NULL, 10);

    int pos;
    for (pos = 0; line[pos] != ';'; pos++) {}
    char expected[512];
    memcpy(expected, line + pos + 1, len - pos - 1);
    expected[len - pos - 1] = '\0';

    jcl_assert(jcu_num2str(buf, sizeof(buf), n), "jc_num2str(%ld)", n);
    jcl_assert(strcmp(buf, expected) == 0, "jc_num2str(%ld) should be '%s', got '%s'", n, expected, buf);
}

int main(void) {
    jcl_assert(jcu_line_parser("../../../../util/num2str_data.txt", reader), "Can not read file");
}
