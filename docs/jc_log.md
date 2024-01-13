# Documentation [jc_log.h](../jc_log.h)

jc_log.h is a
[stb-style](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt)
header only library.
This means which means the library is contained in a single header file.
You can easily add the header file to a project or build system,
without having to worry about what build other build systems.

## Basic Usage
```C
#define JC_LOG_IMPLEMENTATION   // Only do this in one C / C++ file
#include "jc_log.h"

int main(void) {
    trace("TRACE: %d", 3);
    info("%s", "INFO");
    warn("%s: %d", "WARN", 50);
    error("ERROR");
    die("exiting the program");
}
```


Possible Output:

![jc_log_basic_usage.png](https://github.com/Jonas-205/jlibc/blob/main/docs/jc_log_basic_usage.png)

## Prefix
All functions / macros included in this library have the prefix `jcl_`
to avoid name conflicts with other libraries.
This includes the main logging function, e.g. `jcl_trace`.
The logging functions also have short names, e.g. `trace`,
as they appear relatively often. If you want to disable these short names
simply `#define JC_LOG_NO_SHORT_NAMES` before including the header.

## Logging Macros
The main feature of the library are the logging functions
`trace`, `info`, `warn`, `error` and `die`.
They print out their message using the `printf` formatting rules, include
some helpful information and add some color, to easily find errors / warnings.

You can also call 'jc_prefix(log_level)' to just print out the fancy prefix (without a newline)
and then use printf to format your strings.

`die` also stops the program and indicates that an error has occurred.

If you want to override any of these macros you can override their long versions.
E.g.:
```C
#define jcl_trace(...) way_better_logging_function(__VA_ARGS__)
#include "jc_log.h"
```

## Assert
You can use `jcl_assert` similar to normal asserts.
Though this version prints out a message in the libraries style
in addition to stopping the program.

Do not put functions with side effects into asserts.
When asserts are disabled, these functions will not be run.

```C
jcl_assert(ptr != NULL, "Pointer is NULL, Other Variable: %d", other_variable);
```

## Related Function
### Coloring the shell
This library uses [ANSI escape code](https://en.wikipedia.org/wiki/ANSI_escape_code)
to generate color in the terminal. To make this easier there are some
helper functions (see [colored_io](https://github.com/Jonas-205/jlibc/blob/main/examples/log/colored_io.c)):

```C
int jcl_color_attribute(FILE *out, JclColorAttribute attr)
int jcl_color_fg(FILE *out, JclColor color);
int jcl_color_bg(FILE *out, JclColor color);
int jcl_color_set(FILE *out, JclColor fg, JclColor bg, JclColorAttribute attr);
int jcl_color_reset(FILE *out);
```

---

### Log Level
Sometimes it is usefull to only print more important messages,
you can set the log level using `jcl_level` and get the current
level using `jcl_level_current`.
Setting the level to e.g. `JCL_LEVEL_WARN`
will mean that only warnings and above will be printed, no `trace` or `info` messages.
`die` and `assert` messages will always be printed.

### Temporarly disable logging
Sometimes it is usefullt to disable logging for a while.
(Maybe you only want it on for a specific part of the code).
Using `jcl_toggle` you can activate and deactive logging durring runtime.
Using `jcl_active` you can querry the current state.

This will not disable `die` and `assert`.

### Log level to string
Using `jcl_level_str` you can convert a log level enum value to a string (const char *).

### Log file
Sometimes it is usefull to also log to a file, in addition to logging to the shell.
Using `jcl_file` you can open a log file that all messages will be mirrored to.
As color does not really work in log files, only the messages with no color
are written to it.

To close the log file call `jcl_file_close`.

## Compile Options
### JC_LOG_STATIC
Makes all functions static, only use this if you know what you are doing, as it can lead to larger programs otherwise.

### JC_LOG_DISABLE
Disable all logging at compile time. This can not be undone at runtime,
as it completly removes the logging code from the binary.
`die` and `assert` still work and print there messages to `JC_LOG_STDERR`,
but without any formatting (other than the normal printf one).

Be aware this completly removes the logging calls, so similar to disabling asserts,
this can remove function calls if used improperly.

### JC_LOG_DISABLE_ASSERTS
Disables the `jcl_assert` function. Be carefull with side effects:
```C
bool function_with_side_effects(void) {
    return 0 < printf("Printing...");
}

int main() {
    jcl_assert(function_with_side_effects(), "printf failed");
}
```
If JC_LOG_DISABLE_ASSERTS is defined,
The `jcl_assert` statement will be completly removed and
the function with side effects will not be called.

### JC_LOG_STDOUT / JC_LOG_STDERR
`trace` and `info` write to `stdout`, `warn`, `error` and `die` write to `stderr`.
If you want them to write somewhere else (e.g. a file pointer) simply override `JC_LOG_STDOUT` / `JC_LOG_STDERR`.
```C
#define JC_LOG_STDERR stdout  // now all logging functions write to stdout
#include "jc_log.h"
```

### JC_LOG_DISABLE_COLORS
This library uses ANSI escape characters to generate color in the terminal.
Some terminals do not support this, which leads to quite ugly text.
You can disable colors using this macro.
