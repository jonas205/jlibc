/* jcu_util - v0.0.1

Do this:
    #define JC_UTIL_IMPLEMENTATION
before you include this file in *one* C or C++ file to create the implementation.

// i.e. it should look like this:
#include ...
#include ...
#include ...
#define JC_UTIL_IMPLEMENTATION
#include "jcu_util.h"

QUICK NOTES:
    - I mostly try to follow the stb-style in this library
        (https://github.com/nothings/stb/blob/master/docs/stb_howto.txt)
    - jcu_getdelim and jcu_getline implement the posix functions, be careful as they use malloc / realloc

THINGS YOU CAN DEFINE:
    DEFINE EVERYWHERE:
        - JCU_LOG_STATIC             make all functions static, only use this if you know what you are doing
    DEFINE IN SAME FILE AS JC_UTIL_IMPLEMENTATION:

LICENSE:
    See end of file for license information
*/


#ifndef JC_UTIL_H_
#define JC_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#if defined(_WIN32) || defined(WIN32)
#define JC_UTIL_WINDOWS
#elif __APPLE__
#define JC_UTIL_APPLE
#endif

#ifndef JC_UTIL_DEF
#ifdef JC_UTIL_STATIC
#define JC_UTIL_DEF static
#else
#define JC_UTIL_DEF extern
#endif
#endif

#ifdef JC_UTIL_WINDOWS
#define jcu_force_inline static __forceinline
#else
#define jcu_force_inline static __attribute__((always_inline)) inline
#endif

// TODO(me): Once C23 comes out, check for it and use its feature if possible
#ifndef jcu_warn_unused
#ifdef JC_UTIL_WINDOWS  // afaik windows does not support checking for unused results
#define jcu_warn_unused
#else
#define jcu_warn_unused __attribute__((warn_unused_result))
#endif
#endif

#ifndef JC_UTIL_APPLE
typedef int64_t ssize_t;
#endif


jcu_force_inline bool jcu_is_number(char c) {
    return c >= '0' && c <= '9';
}

jcu_force_inline bool jcu_is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

jcu_force_inline bool jcu_is_lower(char c) {
    return c >= 'a' && c <= 'z';
}

jcu_force_inline bool jcu_is_upper(char c) {
    return c >= 'A' && c <= 'Z';
}

jcu_force_inline bool jcu_is_alpha(char c) {
    return jcu_is_lower(c) || jcu_is_upper(c);
}

jcu_force_inline bool jcu_is_alphanum(char c) {
    return jcu_is_alpha(c) || jcu_is_number(c);
}

JC_UTIL_DEF int64_t jcu_gcd(int64_t a, int64_t b);
jcu_force_inline int64_t jcu_lcm(int64_t a, int64_t b) {
    return (a / jcu_gcd(a, b)) * b;
}
JC_UTIL_DEF int64_t jcu_lcm_arr(int64_t *arr, size_t size);  // returns -1 if size is 0

//  returns false if buffer was to small, for buffers >= 256 always true
JC_UTIL_DEF jcu_warn_unused bool jcu_num2str(char *buf, size_t buf_size, int64_t n);
JC_UTIL_DEF jcu_warn_unused bool jcu_str2num(const char *n, int64_t *out);

typedef void (*jcu_line_reader)(const char *line, ssize_t len);
// Reads a file line by line and gives the lines to the reader function
// This uses getline, which uses malloc / realloc !!!
JC_UTIL_DEF jcu_warn_unused bool jcu_line_parser(const char *path, jcu_line_reader reader);

// Custom implementation of posix getdelim, uses malloc / realloc !!!
JC_UTIL_DEF jcu_warn_unused ssize_t jcu_getdelim(char **buf, size_t *buf_size, int delimiter, FILE *fp);
// Custom implementation of posix getline, uses malloc / realloc !!!
jcu_force_inline jcu_warn_unused ssize_t jcu_getline(char **buf, size_t *buf_size, FILE *fp) {
    return jcu_getdelim(buf, buf_size, '\n', fp);
}

#ifdef __cplusplus
}
#endif
//
//
////   end header file   /////////////////////////////////////////////////////
#endif  // JC_UTIL_H_

#ifdef JC_UTIL_IMPLEMENTATION
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

JC_UTIL_DEF int64_t jcu_gcd(int64_t a, int64_t b) {
    if (b == 0) {
        return a;
    }
    return jcu_gcd(b, a % b);
}

JC_UTIL_DEF int64_t jcu_lcm_arr(int64_t *arr, size_t size) {
    if (size <= 0) {
        return -1;
    }

    int64_t erg = arr[0];
    for (size_t i = 1; i < size; i++) {
        erg = jcu_lcm(erg, arr[i]);
    }

    return erg;
}

static bool jcui__num2str(char *buf, size_t buf_size, size_t pos, int64_t n) {
    if (n == INT64_MIN) {
        n += 8;
        if (!jcui__num2str(buf, buf_size, pos, n)) {
            return false;
        }
        size_t len = strlen(buf);
        size_t written = snprintf(buf + len, buf_size - len, " and eight");
        return written < buf_size;
    }

    if (n < 0) {
        size_t written = snprintf(buf, buf_size, "minus ");
        if (written >= buf_size) {
            return false;
        }
        return jcui__num2str(buf, buf_size, 6, -n);
    }
    if (pos >= buf_size) {
        return false;
    }

    size_t written;
    bool is_simple = true;
    switch (n) {
        case 0:
            written = snprintf(buf, buf_size, "zero");
            break;
        case 1:
            written = snprintf(buf + pos, buf_size - pos, "one");
            break;
        case 2:
            written = snprintf(buf + pos, buf_size - pos, "two");
            break;
        case 3:
            written = snprintf(buf + pos, buf_size - pos, "three");
            break;
        case 4:
            written = snprintf(buf + pos, buf_size - pos, "four");
            break;
        case 5:
            written = snprintf(buf + pos, buf_size - pos, "five");
            break;
        case 6:
            written = snprintf(buf + pos, buf_size - pos, "six");
            break;
        case 7:
            written = snprintf(buf + pos, buf_size - pos, "seven");
            break;
        case 8:
            written = snprintf(buf + pos, buf_size - pos, "eight");
            break;
        case 9:
            written = snprintf(buf + pos, buf_size - pos, "nine");
            break;
        case 10:
            written = snprintf(buf + pos, buf_size - pos, "ten");
            break;
        case 11:
            written = snprintf(buf + pos, buf_size - pos, "eleven");
            break;
        case 12:
            written = snprintf(buf + pos, buf_size - pos, "twelve");
            break;
        case 13:
            written = snprintf(buf + pos, buf_size - pos, "thirteen");
            break;
        case 14:
            written = snprintf(buf + pos, buf_size - pos, "fourteen");
            break;
        case 15:
            written = snprintf(buf + pos, buf_size - pos, "fifteen");
            break;
        case 16:
            written = snprintf(buf + pos, buf_size - pos, "sixteen");
            break;
        case 17:
            written = snprintf(buf + pos, buf_size - pos, "seventeen");
            break;
        case 18:
            written = snprintf(buf + pos, buf_size - pos, "eighteen");
            break;
        case 19:
            written = snprintf(buf + pos, buf_size - pos, "nineteen");
            break;
        case 20:
            written = snprintf(buf + pos, buf_size - pos, "twenty");
            break;
        case 30:
            written = snprintf(buf + pos, buf_size - pos, "thirty");
            break;
        case 40:
            written = snprintf(buf + pos, buf_size - pos, "forty");
            break;
        case 50:
            written = snprintf(buf + pos, buf_size - pos, "fifty");
            break;
        case 60:
            written = snprintf(buf + pos, buf_size - pos, "sixty");
            break;
        case 70:
            written = snprintf(buf + pos, buf_size - pos, "seventy");
            break;
        case 80:
            written = snprintf(buf + pos, buf_size - pos, "eighty");
            break;
        case 90:
            written = snprintf(buf + pos, buf_size - pos, "ninety");
            break;
        default:
            is_simple = false;
            written = 0;
            break;
    }
    if (is_simple) {
        return written < buf_size;
    }

    const int64_t thousand = 1000;
    const int64_t million = 1000000;
    const int64_t billion = 1000000000;
    const int64_t trillion = 1000000000000;
    const int64_t quadrillion = 1000000000000000;
    const int64_t quintillion = 1000000000000000000;

    if (n < 100) {
        uint32_t tens = n / 10;
        uint32_t ones = n % 10;

        char buf_tens[32];
        char buf_ones[32];

        if (!jcui__num2str(buf_tens, sizeof(buf_tens), 0, tens * 10)) {
            return false;
        }
        if (ones == 0) {
            written = snprintf(buf + pos, buf_size - pos, "%s", buf_tens);
        } else {
            if (!jcui__num2str(buf_ones, sizeof(buf_ones), 0, ones)) {
                return false;
            }

            written = snprintf(buf + pos, buf_size - pos, "%s-%s", buf_tens, buf_ones);
        }
    } else if (n < thousand) {
        uint32_t hundreds = n / 100;
        uint32_t tens = n % 100;

        char buf_hundreds[32];
        char buf_tens[32];

        if (!jcui__num2str(buf_hundreds, sizeof(buf_hundreds), 0, hundreds)) {
            return false;
        }
        if (tens == 0) {
            written = snprintf(buf + pos, buf_size - pos, "%s hundred", buf_hundreds);
        } else {
            if (!jcui__num2str(buf_tens, sizeof(buf_tens), 0, tens)) {
                return false;
            }

            written = snprintf(buf + pos, buf_size - pos, "%s hundred and %s", buf_hundreds, buf_tens);
        }
    } else if (n < million) {
        uint32_t thousands = n / thousand;
        uint32_t hundreds = n % thousand;

        char buf_thousands[256];
        char buf_hundreds[256];

        if (!jcui__num2str(buf_thousands, sizeof(buf_thousands), 0, thousands)) {
            return false;
        }
        if (hundreds == 0) {
            written = snprintf(buf + pos, buf_size - pos, "%s thousand", buf_thousands);
        } else {
            if (!jcui__num2str(buf_hundreds, sizeof(buf_hundreds), 0, hundreds)) {
                return false;
            }

            if (hundreds < 100) {
                written = snprintf(buf + pos, buf_size - pos, "%s thousand and %s", buf_thousands, buf_hundreds);
            } else {
                written = snprintf(buf + pos, buf_size - pos, "%s thousand, %s", buf_thousands, buf_hundreds);
            }
        }
    } else if (n < billion) {
        uint32_t millions = n / million;
        uint32_t thousands = n % million;

        char buf_millions[256];
        char buf_thousands[256];

        if (!jcui__num2str(buf_millions, sizeof(buf_millions), 0, millions)) {
            return false;
        }
        if (thousands == 0) {
            written = snprintf(buf + pos, buf_size - pos, "%s million", buf_millions);
        } else {
            if (!jcui__num2str(buf_thousands, sizeof(buf_thousands), 0, thousands)) {
                return false;
            }

            if (thousands < 100) {
                written = snprintf(buf + pos, buf_size - pos, "%s million and %s", buf_millions, buf_thousands);
            } else {
                written = snprintf(buf + pos, buf_size - pos, "%s million, %s", buf_millions, buf_thousands);
            }
        }
    } else if (n < trillion) {
        int64_t billions = n / billion;
        int64_t millions = n % billion;

        char buf_billions[256];
        char buf_millions[256];

        if (!jcui__num2str(buf_billions, sizeof(buf_billions), 0, billions)) {
            return false;
        }
        if (millions == 0) {
            written = snprintf(buf + pos, buf_size - pos, "%s billion", buf_billions);
        } else {
            if (!jcui__num2str(buf_millions, sizeof(buf_millions), 0, millions)) {
                return false;
            }

            if (millions < 100) {
                written = snprintf(buf + pos, buf_size - pos, "%s billion and %s", buf_billions, buf_millions);
            } else {
                written = snprintf(buf + pos, buf_size - pos, "%s billion, %s", buf_billions, buf_millions);
            }
        }
    } else if (n < quadrillion) {
        int64_t trillions = n / trillion;
        int64_t billions = n % trillion;

        char buf_trillions[256];
        char buf_billions[256];

        if (!jcui__num2str(buf_trillions, sizeof(buf_trillions), 0, trillions)) {
            return false;
        }
        if (billions == 0) {
            written = snprintf(buf + pos, buf_size - pos, "%s trillion", buf_trillions);
        } else {
            if (!jcui__num2str(buf_billions, sizeof(buf_billions), 0, billions)) {
                return false;
            }

            if (billions < 100) {
                written = snprintf(buf + pos, buf_size - pos, "%s trillion and %s", buf_trillions, buf_billions);
            } else {
                written = snprintf(buf + pos, buf_size - pos, "%s trillion, %s", buf_trillions, buf_billions);
            }
        }
    } else if (n < quintillion) {
        int64_t quadrillions = n / quadrillion;
        int64_t trillions = n % quadrillion;

        char buf_quadrillions[256];
        char buf_trillions[256];

        if (!jcui__num2str(buf_quadrillions, sizeof(buf_quadrillions), 0, quadrillions)) {
            return false;
        }
        if (trillions == 0) {
            written = snprintf(buf + pos, buf_size - pos, "%s quadrillion", buf_quadrillions);
        } else {
            if (!jcui__num2str(buf_trillions, sizeof(buf_trillions), 0, trillions)) {
                return false;
            }

            if (trillions < 100) {
                written = snprintf(buf + pos, buf_size - pos, "%s quadrillion and %s", buf_quadrillions, buf_trillions);
            } else {
                written = snprintf(buf + pos, buf_size - pos, "%s quadrillion, %s", buf_quadrillions, buf_trillions);
            }
        }
    } else {
        int64_t quintillions = n / quintillion;
        int64_t quadrillions = n % quintillion;

        char buf_quintillions[256];
        char buf_quadrillions[256];

        if (!jcui__num2str(buf_quintillions, sizeof(buf_quintillions), 0, quintillions)) {
            return false;
        }
        if (quadrillions == 0) {
            written = snprintf(buf + pos, buf_size - pos, "%s quintillion", buf_quintillions);
        } else {
            if (!jcui__num2str(buf_quadrillions, sizeof(buf_quadrillions), 0, quadrillions)) {
                return false;
            }

            if (quadrillions < 100) {
                written = snprintf(buf + pos, buf_size - pos,
                        "%s quintillion and %s", buf_quintillions, buf_quadrillions);
            } else {
                written = snprintf(buf + pos, buf_size - pos,
                        "%s quintillion, %s", buf_quintillions, buf_quadrillions);
            }
        }
    }
    return written < buf_size;
}

JC_UTIL_DEF bool jcu_num2str(char *buf, size_t buf_size, int64_t n) {
    return jcui__num2str(buf, buf_size, 0, n);
}

typedef enum {
    JCU_NT_MINUS,
    JCU_NT_ZERO,
    JCU_NT_ONE,
    JCU_NT_TWO,
    JCU_NT_THREE,
    JCU_NT_FOUR,
    JCU_NT_FIVE,
    JCU_NT_SIX,
    JCU_NT_SEVEN,
    JCU_NT_EIGHT,
    JCU_NT_NINE,
    JCU_NT_TEN,
    JCU_NT_ELEVEN,
    JCU_NT_TWELVE,
    JCU_NT_THIRTEEN,
    JCU_NT_FOURTEEN,
    JCU_NT_FIFTEEN,
    JCU_NT_SIXTEEN,
    JCU_NT_SEVENTEEN,
    JCU_NT_EIGHTEEN,
    JCU_NT_NINETEEN,
    JCU_NT_TWENTY,
    JCU_NT_THIRTY,
    JCU_NT_FORTY,
    JCU_NT_FIFTY,
    JCU_NT_SIXTY,
    JCU_NT_SEVENTY,
    JCU_NT_EIGHTY,
    JCU_NT_NINETY,
    JCU_NT_HUNDRED,
    JCU_NT_THOUSAND,
    JCU_NT_MILLION,
    JCU_NT_BILLION,
    JCU_NT_TRILLION,
    JCU_NT_QUADRILLION,
    JCU_NT_QUINTILLION,
} JCI__NumberTokens;

JC_UTIL_DEF bool jcu_str2num(const char *num, int64_t *out) {
    JCI__NumberTokens tokens[256];
    size_t token_count = 0;

    size_t len = strlen(num);
    size_t i = 0;

    while (i < len) {
        if (jcu_is_whitespace(num[i]) || num[i] == ',' || num[i] == '-') {
            i++;
            continue;
        } else if (strncmp("and", num + i, 3) == 0) {
            i += 3;
            continue;
        }

        if (strncmp("quintillion", num + i, 11) == 0) {
            tokens[token_count++] = JCU_NT_QUINTILLION;
            i += 11;
        } else if (strncmp("quadrillion", num + i, 11) == 0) {
            tokens[token_count++] = JCU_NT_QUADRILLION;
            i += 11;
        } else if (strncmp("trillion", num + i, 8) == 0) {
            tokens[token_count++] = JCU_NT_TRILLION;
            i += 8;
        } else if (strncmp("billion", num + i, 7) == 0) {
            tokens[token_count++] = JCU_NT_BILLION;
            i += 7;
        } else if (strncmp("million", num + i, 7) == 0) {
            tokens[token_count++] = JCU_NT_MILLION;
            i += 7;
        } else if (strncmp("thousand", num + i, 8) == 0) {
            tokens[token_count++] = JCU_NT_THOUSAND;
            i += 8;
        } else if (strncmp("hundred", num + i, 7) == 0) {
            tokens[token_count++] = JCU_NT_HUNDRED;
            i += 7;
        } else if (strncmp("ninety", num + i, 6) == 0) {
            tokens[token_count++] = JCU_NT_NINETY;
            i += 6;
        } else if (strncmp("eighty", num + i, 6) == 0) {
            tokens[token_count++] = JCU_NT_EIGHTY;
            i += 6;
        } else if (strncmp("seventy", num + i, 7) == 0) {
            tokens[token_count++] = JCU_NT_SEVENTY;
            i += 7;
        } else if (strncmp("sixty", num + i, 5) == 0) {
            tokens[token_count++] = JCU_NT_SIXTY;
            i += 5;
        } else if (strncmp("fifty", num + i, 5) == 0) {
            tokens[token_count++] = JCU_NT_FIFTY;
            i += 5;
        } else if (strncmp("forty", num + i, 5) == 0) {
            tokens[token_count++] = JCU_NT_FORTY;
            i += 5;
        } else if (strncmp("thirty", num + i, 6) == 0) {
            tokens[token_count++] = JCU_NT_THIRTY;
            i += 6;
        } else if (strncmp("twenty", num + i, 6) == 0) {
            tokens[token_count++] = JCU_NT_TWENTY;
            i += 6;
        } else if (strncmp("nineteen", num + i, 8) == 0) {
            tokens[token_count++] = JCU_NT_NINETEEN;
            i += 8;
        } else if (strncmp("eighteen", num + i, 8) == 0) {
            tokens[token_count++] = JCU_NT_EIGHTEEN;
            i += 8;
        } else if (strncmp("seventeen", num + i, 9) == 0) {
            tokens[token_count++] = JCU_NT_SEVENTEEN;
            i += 9;
        } else if (strncmp("sixteen", num + i, 7) == 0) {
            tokens[token_count++] = JCU_NT_SIXTEEN;
            i += 7;
        } else if (strncmp("fifteen", num + i, 7) == 0) {
            tokens[token_count++] = JCU_NT_FIFTEEN;
            i += 7;
        } else if (strncmp("fourteen", num + i, 8) == 0) {
            tokens[token_count++] = JCU_NT_FOURTEEN;
            i += 8;
        } else if (strncmp("thirteen", num + i, 8) == 0) {
            tokens[token_count++] = JCU_NT_THIRTEEN;
            i += 8;
        } else if (strncmp("twelve", num + i, 6) == 0) {
            tokens[token_count++] = JCU_NT_TWELVE;
            i += 6;
        } else if (strncmp("eleven", num + i, 6) == 0) {
            tokens[token_count++] = JCU_NT_ELEVEN;
            i += 6;
        } else if (strncmp("ten", num + i, 3) == 0) {
            tokens[token_count++] = JCU_NT_TEN;
            i += 3;
        } else if (strncmp("nine", num + i, 4) == 0) {
            tokens[token_count++] = JCU_NT_NINE;
            i += 4;
        } else if (strncmp("eight", num + i, 5) == 0) {
            tokens[token_count++] = JCU_NT_EIGHT;
            i += 5;
        } else if (strncmp("seven", num + i, 5) == 0) {
            tokens[token_count++] = JCU_NT_SEVEN;
            i += 5;
        } else if (strncmp("six", num + i, 3) == 0) {
            tokens[token_count++] = JCU_NT_SIX;
            i += 3;
        } else if (strncmp("five", num + i, 4) == 0) {
            tokens[token_count++] = JCU_NT_FIVE;
            i += 4;
        } else if (strncmp("four", num + i, 4) == 0) {
            tokens[token_count++] = JCU_NT_FOUR;
            i += 4;
        } else if (strncmp("three", num + i, 5) == 0) {
            tokens[token_count++] = JCU_NT_THREE;
            i += 5;
        } else if (strncmp("two", num + i, 3) == 0) {
            tokens[token_count++] = JCU_NT_TWO;
            i += 3;
        } else if (strncmp("one", num + i, 3) == 0) {
            tokens[token_count++] = JCU_NT_ONE;
            i += 3;
        } else if (strncmp("zero", num + i, 4) == 0) {
            tokens[token_count++] = JCU_NT_ZERO;
            i += 4;
        } else if (strncmp("minus", num + i, 5) == 0) {
            tokens[token_count++] = JCU_NT_MINUS;
            i += 5;
        } else {
            if (token_count == 0) {
                return false;
            } else if (token_count == 1 && tokens[0] == JCU_NT_MINUS) {
                return false;
            }
            break;
        }
    }

    if (token_count == 0) {
        *out = 0;
        return true;
    }

    int64_t total = 0;
    int64_t n = 0;
    bool negative = false;

    for (size_t i = 0; i < token_count; i++) {
        switch (tokens[i]) {
            case JCU_NT_MINUS:
                if (i != 0) {
                    return false;
                }
                negative = true;
                break;
            case JCU_NT_ZERO:
                if (i != 0 && token_count != 1) {
                    return false;
                }
                break;
            case JCU_NT_ONE:
                n += 1;
                break;
            case JCU_NT_TWO:
                n += 2;
                break;
            case JCU_NT_THREE:
                n += 3;
                break;
            case JCU_NT_FOUR:
                n += 4;
                break;
            case JCU_NT_FIVE:
                n += 5;
                break;
            case JCU_NT_SIX:
                n += 6;
                break;
            case JCU_NT_SEVEN:
                n += 7;
                break;
            case JCU_NT_EIGHT:
                n += 8;
                break;
            case JCU_NT_NINE:
                n += 9;
                break;
            case JCU_NT_TEN:
                n += 10;
                break;
            case JCU_NT_ELEVEN:
                n += 11;
                break;
            case JCU_NT_TWELVE:
                n += 12;
                break;
            case JCU_NT_THIRTEEN:
                n += 13;
                break;
            case JCU_NT_FOURTEEN:
                n += 14;
                break;
            case JCU_NT_FIFTEEN:
                n += 15;
                break;
            case JCU_NT_SIXTEEN:
                n += 16;
                break;
            case JCU_NT_SEVENTEEN:
                n += 17;
                break;
            case JCU_NT_EIGHTEEN:
                n += 18;
                break;
            case JCU_NT_NINETEEN:
                n += 19;
                break;
            case JCU_NT_TWENTY:
                n += 20;
                break;
            case JCU_NT_THIRTY:
                n += 30;
                break;
            case JCU_NT_FORTY:
                n += 40;
                break;
            case JCU_NT_FIFTY:
                n += 50;
                break;
            case JCU_NT_SIXTY:
                n += 60;
                break;
            case JCU_NT_SEVENTY:
                n += 70;
                break;
            case JCU_NT_EIGHTY:
                n += 80;
                break;
            case JCU_NT_NINETY:
                n += 90;
                break;
            case JCU_NT_HUNDRED:
                n *= 100;
                break;
            case JCU_NT_THOUSAND:
                n *= 1000;
                total += n;
                n = 0;
                break;
            case JCU_NT_MILLION:
                n *= 1000000;
                total += n;
                n = 0;
                break;
            case JCU_NT_BILLION:
                n *= 1000000000;
                total += n;
                n = 0;
                break;
            case JCU_NT_TRILLION:
                n *= 1000000000000;
                total += n;
                n = 0;
                break;
            case JCU_NT_QUADRILLION:
                n *= 1000000000000000;
                total += n;
                n = 0;
                break;
            case JCU_NT_QUINTILLION:
                n *= 1000000000000000000;
                total += n;
                n = 0;
                break;
        }
    }
    total += n;
    *out = negative ? -total : total;
    return true;
}

JC_UTIL_DEF bool jcu_line_parser(const char *path, jcu_line_reader reader) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return false;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = jcu_getline(&line, &len, file)) != -1) {
        reader(line, read);
    }

    if (fclose(file)) {
        return false;
    }

    if (line) {
        free(line);
    }

    return true;
}

JC_UTIL_DEF ssize_t jcu_getdelim(char **buf, size_t *buf_size, int delimiter, FILE *fp) {
    if (*buf == NULL || *buf_size == 0) {
        *buf_size = 256;
        *buf = malloc(*buf_size);
        if (*buf == NULL) {
            return -1;
        }
    }

    for (size_t i = 0;; i++) {
        int c = fgetc(fp);

        if (c == -1) {
            if (feof(fp)) {
                if (i == 0) {
                    return -1;
                }
                (*buf)[i] = '\0';
                return i;
            }
            return -1;
        }
        (*buf)[i] = c;

        if (c == delimiter) {
            (*buf)[i + 1] = '\0';
            return i;
        }

        if (i + 2 >= *buf_size) {
            *buf_size *= 2;
            char *new_buf = realloc(*buf, *buf_size);
            if (new_buf == NULL) {
                return -1;
            }
            *buf = new_buf;
        }
    }
}

#ifdef __cplusplus
}
#endif
#endif  // JC_UTIL_IMPLEMENTATION

/*
------------------------------------------------------------------------------
MIT License

Copyright (c) 2024 Jonas Kristen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
*/
