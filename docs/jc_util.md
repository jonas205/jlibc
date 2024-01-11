# Documentation [jc_util.h](../jc_util.h)

jc_util.h is a
[stb-style](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt)
header only library.
This means which means the library is contained in a single header file.
You can easily add the header file to a project or build system,
without having to worry about what build other build systems.

## Basic Usage
```C
#define JC_UTIL_IMPLEMENTATION   // Only do this in one C / C++ file
#include "jc_util.h"

int main(void) {
    char buf[512];
    assert(jcu_num2str(buf, sizeof(buf), 123));
    printf("%s\n", buf);  // one hundred and twenty-three
}
```

## Prefix
All functions / macros included in this library have the prefix `jcu_`
to avoid name conflicts with other libraries.

## Simple Char Functions
A number of simple is char a ... functions are included.

Name | Doc
-|-
jcu_is_number | Char is '0' to '9'
jcu_is_whitespace | Char is ' ', '\t', '\n' or '\r'
jcu_is_lower | Char is lower case letter
jcu_is_upper | Char is upper case letter
jcu_is_alpha | Char is letter
jcu_is_alphanum | Char is letter or number

## Math helpers
Some Math function often used in programming challenges (but not real life).

Name | Doc
-|-
jcu_gcd | Greatest common divisor
jcu_lcm | Least common multiple
jcu_lcm_arr | Least common multiple of an array of numbers

## Posix helpers
Some Posix functions are not available on inferior operating systems (Windows).

Name | Doc
-|-
jcu_getline | Read a line from a file descriptor
jcu_getdelim | Read a line from a file descriptor with a custom delimiter

## Line Parser
A line parser is included to parse lines one by one from a file descriptor.

Usage:
```C
#define JC_UTIL_IMPLEMENTATION
#include "../../jc_util.h"

void reader(const char *line, ssize_t len) {
    printf("Len: %.2ld, Content: %s", len, line);  // newline is included in line
}

int main(void) {
    return !jcu_line_parser("read_lines.c", reader);
}
```
