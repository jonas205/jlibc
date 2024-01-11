#include <string.h>
#define JC_UTIL_IMPLEMENTATION
#include "../../jc_util.h"
#define JC_LOG_IMPLEMENTATION
#include "../../jc_log.h"

static const char *expected[] = {
    "Wow, this is a line",
    "",
    "",
    "nice",
    "",
    "What a cool test",
    "",
    "",
    "",
    "",
    "",
    "",
};

static int i = 0;

void reader(const char *line_orig, ssize_t len) {
    const char *e = expected[i++];
    jcl_assert(len >= 0, "this example should never fail");

    char line[512];
    memcpy(line, line_orig, len);
    line[len] = '\0';

    size_t ulen = len;

    info("'%s' (%ld)", line, ulen);

    jcl_assert(strlen(line) == ulen, "'%s': The len value (%ld) is not the same as strlen(line)(%ld)",
            line, ulen, strlen(line));
    jcl_assert(strlen(e) == ulen, "'%s':The line length (%ld) is not the same as the expected line length (%ld)",
            line, ulen, strlen(e));
    jcl_assert(strcmp(e, line) == 0, "The line '%s' is not the same as the expected line '%s'", line, e);
}

int main(void) {
#ifdef JC_UTIL_WINDOWS
    jcl_assert(jcu_line_parser("..\\..\\tests\\util\\read_lines.txt", reader), "Can not read file");
#else
    jcl_assert(jcu_line_parser("../../../../util/read_lines.txt", reader), "Can not read file");
#endif
}
