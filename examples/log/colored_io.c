#include <string.h>

#define JC_LOG_IMPLEMENTATION
#include "../../jc_log.h"

#define WIDTH 8

int main(void) {
    printf("Colored IO:\n");

    char *colors[] = {
        "default",
        "black",
        "red",
        "green",
        "yellow",
        "blue",
        "magenta",
        "cyan",
        "white",
        "gray",
        "b red",
        "b green",
        "b yellow",
        "b blue",
        "bmagenta",
        "b cyan",
        "b white",
    };

    int color_values[] = {
        JCL_COLOR_DEFAULT,
        JCL_COLOR_BLACK,
        JCL_COLOR_RED,
        JCL_COLOR_GREEN,
        JCL_COLOR_YELLOW,
        JCL_COLOR_BLUE,
        JCL_COLOR_MAGENTA,
        JCL_COLOR_CYAN,
        JCL_COLOR_WHITE,
        JCL_COLOR_GRAY,
        JCL_COLOR_BRIGHT_RED,
        JCL_COLOR_BRIGHT_GREEN,
        JCL_COLOR_BRIGHT_YELLOW,
        JCL_COLOR_BRIGHT_BLUE,
        JCL_COLOR_BRIGHT_MAGENTA,
        JCL_COLOR_BRIGHT_CYAN,
        JCL_COLOR_BRIGHT_WHITE,
    };

    printf("Colors: |");
    for (size_t i = 0; i < sizeof(colors) / sizeof(colors[0]); i++) {
        int len = strlen(colors[i]);
        int padding = WIDTH - len;
        int padding_left = padding / 2;
        int padding_right = padding - padding_left;
        char padding_left_str[WIDTH + 1] = {0};
        char padding_right_str[WIDTH + 1] = {0};
        memset(padding_left_str, ' ', padding_left);
        memset(padding_right_str, ' ', padding_right);

        printf("%s%s%s%c",
                padding_left_str,
                colors[i],
                padding_right_str,
                i == sizeof(colors) / sizeof(colors[0]) - 1 ? '\n' : '|');
    }

    for (size_t i = 0; i < sizeof(colors) / sizeof(colors[0]); i++) {
        int len = strlen(colors[i]);
        int padding = 8 - len;
        int padding_left = padding / 2;
        int padding_right = padding - padding_left;
        char padding_left_str[WIDTH + 1] = {0};
        char padding_right_str[WIDTH + 1] = {0};
        memset(padding_left_str, ' ', padding_left);
        memset(padding_right_str, ' ', padding_right);

        printf("%s%s%s|", padding_left_str, colors[i], padding_right_str);

        for (size_t j = 0; j < sizeof(colors) / sizeof(colors[0]); j++) {
            jcl_color_fg(stdout, color_values[i]);
            jcl_color_bg(stdout, color_values[j]);
            printf("abcdefgh");
            jcl_color_reset(stdout);
            if (j != sizeof(colors) / sizeof(colors[0]) - 1) {
                printf("|");
            }
        }
        printf("\n");
    }

    jcl_color_attribute(stdout, JCL_COLOR_ATTR_BRIGHT);
    printf("Bright:\n");
    jcl_color_reset(stdout);
    jcl_color_attribute(stdout, JCL_COLOR_ATTR_DIM);
    printf("Dim:\n");
    jcl_color_reset(stdout);
    jcl_color_attribute(stdout, JCL_COLOR_ATTR_UNDERSCORE);
    printf("Underline:\n");
    jcl_color_reset(stdout);
    jcl_color_attribute(stdout, JCL_COLOR_ATTR_BLINK);
    printf("Blink:\n");
    jcl_color_reset(stdout);
    jcl_color_attribute(stdout, JCL_COLOR_ATTR_REVERSE);
    printf("Reverse:\n");
    jcl_color_reset(stdout);
    jcl_color_attribute(stdout, JCL_COLOR_ATTR_HIDDEN);
    printf("Hidden:");
    jcl_color_reset(stdout);
    printf("   <-- Hidden\n");
    jcl_color_attribute(stdout, JCL_COLOR_ATTR_STRIKE);
    printf("Strike:\n");
    jcl_color_reset(stdout);
    jcl_color_attribute(stdout, JCL_COLOR_ATTR_DOUBLE_UNDERSCORE);
    printf("Double Underscore:\n");
    jcl_color_reset(stdout);
}
