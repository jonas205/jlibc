#include <stdlib.h>
#include <time.h>

#define JC_UTIL_IMPLEMENTATION
#include "../../jc_util.h"
#define JC_LOG_IMPLEMENTATION
#include "../../jc_log.h"

void test(int64_t n) {
    char buf[512];
    jcl_assert(jcu_num2str(buf, sizeof(buf), n), "jcu_num2str(%ld) should be true", n);
    int64_t n2;

    jcl_assert(jcu_str2num(buf, &n2), "jcu_str2num('%s') should be true", buf);
    jcl_assert(n == n2, "str2num('%s') should be %ld, got %ld", buf, n, n2);
}

void test_special(void) {
    char buf[512];
    snprintf(buf, sizeof(buf), "Hello, world!");
    int64_t n;
    jcl_assert(!jcu_str2num(buf, &n), "jcu_str2num('%s') should be false, was %ld", buf, n);

    snprintf(buf, sizeof(buf), "123");
    jcl_assert(!jcu_str2num(buf, &n), "jcu_str2num('%s') should be false, was %ld", buf, n);

    snprintf(buf, sizeof(buf), "one and something else");
    jcl_assert(jcu_str2num(buf, &n), "jcu_str2num('%s') should be true", buf);
    jcl_assert(n == 1, "jcu_str2num('%s') should be 1, was %ld", buf, n);

    snprintf(buf, sizeof(buf), "an apple and three thousand");
    jcl_assert(!jcu_str2num(buf, &n), "jcu_str2num('%s') should be false, was %ld", buf, n);

    snprintf(buf, sizeof(buf), "one hundred and two and a nice car");
    jcl_assert(jcu_str2num(buf, &n), "jcu_str2num('%s') should be true", buf);
    jcl_assert(n == 102, "jcu_str2num('%s') should be 102, was %ld", buf, n);
}

int main(void) {
    test_special();

    for (int64_t i = -1000; i <= 1000; i++) {
        test(i);
    }

    test(INT64_MIN);
    test(INT64_MAX);

    time_t t;
    srand((unsigned) time(&t));

    for (int64_t i = 0; i < 1000; i++) {
        int64_t n = rand();
        n <<= 32;
        n += (uint32_t) rand();
        test(n);
    }
}
