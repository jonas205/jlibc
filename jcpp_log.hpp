/* jcppl - v1.0.0 - Fancy logging in C

Do this:
    #define JC_LOG_IMPLEMENTATION
before you include this file in *one* C or C++ file to create the
implementation.

// i.e. it should look like this:
#include ...
#include ...
#include ...
#define JC_LOG_IMPLEMENTATION
#include "jcpp_log.h"

QUICK NOTES:
    - I mostly try to follow the stb-style in this library
        (https://github.com/nothings/stb/blob/master/docs/stb_howto.txt)

THINGS YOU CAN DEFINE:
DEFINE EVERYWHERE:
    - JC_LOG_DISABLE            disables all logging (die still works and
prints only the message to stderr)
    - JC_LOG_DISABLE_ASSERTS    disables jlog_assert
    - JC_LOG_NO_SHORT_NAMES     removes short names (trace, info, ...)
    - JC_LOG_OUT         some logging functions use std::out (trace, info), you
can override this here
    - JC_LOG_ERR         some logging functions use std::cerr (warn, error,
die), you can override this here

LICENSE:
    See end of file for license information
*/

#ifndef JC_LOG_HPP
#define JC_LOG_HPP

#include <fstream>
#include <iostream>
#include <ostream>
#include <utility>

#ifndef JC_LOG_OUT
#define JC_LOG_OUT std::cout
#endif
#ifndef JC_LOG_ERR
#define JC_LOG_ERR std::cerr
#endif

#ifndef JC_LOG_DISABLE
namespace JCPP::LOG {
enum class LogLevel {
    TRACE = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3,
    DIE = 4,
};

class Logger {
 public:
    explicit Logger(const char *filename) : m_file(filename) {
        if (S_LOGGER != nullptr) {
            JC_LOG_ERR << "Logger already Initialized" << std::endl;
            exit(1);
        }
        S_LOGGER = this;
    }
    ~Logger() { m_file.close(); }

    inline void log_die() {
        this->~Logger();
        exit(1);
    }

    void log_level(LogLevel level) { m_level = level; }
    LogLevel level() { return m_level; }

    inline Logger &cout() {
        m_msg_level = LogLevel::TRACE;
        m_cout = true;
        return *this;
    }
    inline Logger &cerr() {
        m_msg_level = LogLevel::ERROR;
        m_cout = false;
        return *this;
    }

    inline Logger &prefix(const LogLevel &level, const char *file, int line) {
        m_msg_level = level;
        if (level < m_level) {
            return *this;
        }
        m_cout = (level == LogLevel::ERROR || level == LogLevel::WARN);
        auto &out = (m_cout) ? JC_LOG_OUT : JC_LOG_ERR;

        // current time in hh:mm:ss
        time_t now = time(0);
        tm *ltm = localtime(&now);
        char time[9];
        strftime(time, 9, "%T", ltm);
        out << "\033[90m" << time << ' ';  // gray
        m_file << time << ' ';

        switch (level) {
            case LogLevel::TRACE:
                out << "\033[0m";  // default
                out << "trace";
                m_file << "trace";
                break;
            case LogLevel::INFO:
                out << "\033[94m";  // bright blue
                out << "info ";
                m_file << "info ";
                break;
            case LogLevel::WARN:
                out << "\033[93m";  // bright yellow
                out << "warn ";
                m_file << "warn ";
                break;
            case LogLevel::ERROR:
                out << "\033[31m";  // red
                out << "error";
                m_file << "error";
                break;
            case LogLevel::DIE:
                out << "\033[31m";  // red
                out << "die";
                m_file << "die";
                break;
        }

        out << " \033[90m" << file << ':' << line;
        m_file << " " << file << ':' << line;

        out << "\033[0m ";
        m_file << ' ';
        return *this;
    }

    template <typename T>
    Logger &operator<<(const T &t) {
        if (m_msg_level < m_level) {
            return *this;
        }

        std::ostream &out = (m_cout) ? JC_LOG_OUT : JC_LOG_ERR;

        out << t;
        m_file << t;
        return *this;
    }

    template <typename... T>
    Logger &args(T &&...args) {
        return args_impl(std::forward<T>(args)...);
    }

    static inline Logger &get_static_logger() { return *S_LOGGER; }

 private:
    std::ofstream m_file;
    std::ostream m_null_stream{0};
    bool m_cout = true;

    LogLevel m_level = LogLevel::TRACE, m_msg_level = LogLevel::TRACE;

    template <typename T, typename... U>
    Logger &args_impl(const T &arg, const U &...args) {
        *this << arg;
        if constexpr (sizeof...(args) > 0) {
            *this << ", ";
            return args_impl(args...);
        }
        return *this;
    }

    static Logger *S_LOGGER;
};

}  // namespace JCPP::LOG

#define JC_COUT ::JCPP::LOG::Logger::get_static_logger().cout()
#define JC_CERR ::JCPP::LOG::Logger::get_static_logger().cerr()
#define JC_TRACE(msg)                                     \
    ::JCPP::LOG::Logger::get_static_logger().prefix(      \
        ::JCPP::LOG::LogLevel::TRACE, __FILE__, __LINE__) \
        << msg << '\n';
#define JC_TRACE_FN()                                     \
    ::JCPP::LOG::Logger::get_static_logger().prefix(      \
        ::JCPP::LOG::LogLevel::TRACE, __FILE__, __LINE__) \
        << __func__ << "()\n"
#define JC_TRACE_FN_ARGS(...)                              \
    (::JCPP::LOG::Logger::get_static_logger().prefix(      \
         ::JCPP::LOG::LogLevel::TRACE, __FILE__, __LINE__) \
     << __func__ << '(')                                   \
            .args(__VA_ARGS__)                             \
        << ")\n"

#define JC_TRACE_PREFIX                              \
    ::JCPP::LOG::Logger::get_static_logger().prefix( \
        ::JCPP::LOG::LogLevel::TRACE, __FILE__, __LINE__)
#define JC_INFO(msg)                                     \
    ::JCPP::LOG::Logger::get_static_logger().prefix(     \
        ::JCPP::LOG::LogLevel::INFO, __FILE__, __LINE__) \
        << msg << '\n';
#define JC_INFO_PREFIX                               \
    ::JCPP::LOG::Logger::get_static_logger().prefix( \
        ::JCPP::LOG::LogLevel::INFO, __FILE__, __LINE__)
#define JC_WARN(msg)                                     \
    ::JCPP::LOG::Logger::get_static_logger().prefix(     \
        ::JCPP::LOG::LogLevel::WARN, __FILE__, __LINE__) \
        << msg << '\n';
#define JC_WARN_PREFIX                               \
    ::JCPP::LOG::Logger::get_static_logger().prefix( \
        ::JCPP::LOG::LogLevel::WARN, __FILE__, __LINE__)
#define JC_ERROR(msg)                                     \
    ::JCPP::LOG::Logger::get_static_logger().prefix(      \
        ::JCPP::LOG::LogLevel::ERROR, __FILE__, __LINE__) \
        << msg << '\n';
#define JC_ERROR_PREFIX                              \
    ::JCPP::LOG::Logger::get_static_logger().prefix( \
        ::JCPP::LOG::LogLevel::ERROR, __FILE__, __LINE__)
#define JC_DIE(msg)                                      \
    (::JCPP::LOG::Logger::get_static_logger().prefix(    \
         ::JCPP::LOG::LogLevel::DIE, __FILE__, __LINE__) \
     << msg << '\n')                                     \
        .log_die();

#ifndef JC_LOG_NO_SHORT_NAMES
#define trace JC_TRACE
#define trace_fn JC_TRACE_FN
#define trace_fn_args JC_TRACE_FN_ARGS
#define trace_prefix JC_TRACE_PREFIX
#define info JC_INFO
#define info_prefix JC_INFO_PREFIX
#define warn JC_WARN
#define warn_prefix JC_WARN_PREFIX
#define error JC_ERROR
#define error_prefix JC_ERROR_PREFIX
#define die JC_DIE
#endif

#ifdef JC_LOG_DISABLE_ASSERTS
#define JC_ASSERT(cond, msg)
#else
#define JC_ASSERT(cond, msg)                                \
    if (!(cond)) {                                          \
        ::JCPP::LOG::Logger::get_static_logger().prefix(    \
            ::JCPP::LOG::LogLevel::DIE, __FILE__, __LINE__) \
            << "Assertion failed: " << #cond << '\n';       \
        JC_DIE(msg);                                        \
    }
#endif

#else
namespace JCPP::LOG {
enum class LogLevel {
    TRACE,
    INFO,
    WARN,
    ERROR,
    DIE,
};
class Logger {
 public:
    explicit Logger(const char *_filename) {}

    void log_level(LogLevel level) { m_level = level; }
    LogLevel level() { return m_level; }

 private:
    LogLevel m_level = LogLevel::TRACE;
    static Logger *S_LOGGER;
};

}  // namespace JCPP::LOG

#define JC_COUT std::ostream(0)
#define JC_CERR std::ostream(0)
#define JC_TRACE(msg)
#define JC_TRACE_FN()
#define JC_TRACE_FN_ARGS(...)
#define JC_TRACE_PREFIX std::ostream(0)
#define JC_INFO(msg)
#define JC_INFO_PREFIX std::ostream(0)
#define JC_WARN(msg)
#define JC_WARN_PREFIX std::ostream(0)
#define JC_ERROR(msg)
#define JC_ERROR_PREFIX std::ostream(0)
#define JC_DIE(msg)                 \
    do { std::cerr << msg << '\n'); \
        exit(1);                    \
    } while (0)

#ifndef JC_LOG_NO_SHORT_NAMES
#define trace JC_TRACE
#define trace_fn JC_TRACE_FN
#define trace_fn_args JC_TRACE_FN_ARGS
#define trace_prefix JC_TRACE_PREFIX
#define info JC_INFO
#define info_prefix JC_INFO_PREFIX
#define warn JC_WARN
#define warn_prefix JC_WARN_PREFIX
#define error JC_ERROR
#define error_prefix JC_ERROR_PREFIX
#define die JC_DIE
#endif

#ifdef JC_LOG_DISABLE_ASSERTS
#define JC_ASSERT(cond, msg)
#else
#define JC_ASSERT(cond, msg)                                \
    if (!(cond)) {                                          \
        std::cerr << "Assertion failed: " << #cond << '\n'; \
        std::cerr << msg << '\n';                           \
        exit(1);                                            \
    }
#endif

#endif  // JC_LOG_DISABLE

#endif  // HEADER GUARD

#ifdef JC_LOG_IMPLEMENTATION
namespace JCPP::LOG {
class Logger;
Logger *Logger::S_LOGGER = nullptr;
}  // namespace JCPP::LOG
#endif

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
