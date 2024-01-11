#define JC_UTIL_IMPLEMENTATION
#include "../../jc_util.h"
#define JC_LOG_IMPLEMENTATION
#include "../../jc_log.h"

void test_char(void) {
    for (size_t i = 0; i < 256; i++) {
        char c = (char)i;
        bool is_number = c >= '0' && c <= '9';
        bool is_whitespace = c == ' ' || c == '\t' || c == '\n' || c == '\r';
        bool is_upper = c >= 'A' && c <= 'Z';
        bool is_lower = c >= 'a' && c <= 'z';
        bool is_alpha = is_upper || is_lower;
        bool is_alphanum = is_alpha || is_number;

        jcl_assert(jcu_is_number(c) == is_number, "jcu_is_number('%c') should be %s", c, is_number ? "true" : "false");
        jcl_assert(jcu_is_whitespace(c) == is_whitespace,
                "jcu_is_whitespace('%c') should be %s", c, is_whitespace ? "true" : "false");
        jcl_assert(jcu_is_upper(c) == is_upper, "jcu_is_upper('%c') should be %s", c, is_upper ? "true" : "false");
        jcl_assert(jcu_is_lower(c) == is_lower, "jcu_is_lower('%c') should be %s", c, is_lower ? "true" : "false");
        jcl_assert(jcu_is_alpha(c) == is_alpha, "jcu_is_alpha('%c') should be %s", c, is_alpha ? "true" : "false");
        jcl_assert(jcu_is_alphanum(c) == is_alphanum,
                "jcu_is_alphanum('%c') should be %s", c, is_alphanum ? "true" : "false");
    }
}

void test_gcd(void) {
    jcl_assert(jcu_gcd(100, 15) == 5, "gcd(100, 15) should be 5");
    jcl_assert(jcu_gcd((unsigned)100, 15) == 5, "gcd(100, 15) should be 5");
    jcl_assert(jcu_gcd(100, (unsigned)15) == 5, "gcd(100, 15) should be 5");
    jcl_assert(jcu_gcd((unsigned)100, (unsigned)15) == 5, "gcd(100, 15) should be 5");
}

int main(void) {
    test_char();
    test_gcd();
}
