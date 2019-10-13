#pragma once
#include <mutex>
#include "fmt/core.h"

class Log {
    public:
    template<typename... Args>
    static void error(const char* format, const Args&... args) {
        log_impl("Error:", format, fmt::make_format_args(args...));
    }

    template<typename... Args>
    static void warn(const char* format, const Args&... args) {
        log_impl("Warning:", format, fmt::make_format_args(args...));
    }

    template<typename... Args>
    static void info(const char* format, const Args&... args) {
        log_impl("", format, fmt::make_format_args(args...));
    }

    private:
    static void log_impl(const char* prefix, const char* format,
                         fmt::format_args args);
};
