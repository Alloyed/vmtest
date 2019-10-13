#include "Log.hpp"
#include "fmt/format.h"

namespace {
std::mutex g_printMutex;
}

void Log::log_impl(const char* prefix, const char* format,
                   fmt::format_args args) {
    std::unique_lock<std::mutex> lock(g_printMutex);
    fmt::print(prefix);
    fmt::vprint(format, args);
    fmt::print("\n");
}
