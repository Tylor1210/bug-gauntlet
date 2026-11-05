#include "log.h"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <functional>
#include <mutex>
#include <thread>

namespace logging {
namespace {

std::atomic<int> g_level{static_cast<int>(LogLevel::Info)};
std::mutex g_mu;
const auto g_start = std::chrono::steady_clock::now();

const char* name(LogLevel l) {
    switch (l) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO ";
        case LogLevel::Warn: return "WARN ";
        case LogLevel::Error: return "ERROR";
    }
    return "?";
}

}  // namespace

void set_level(LogLevel level) { g_level = static_cast<int>(level); }

void write(LogLevel level, const std::string& component, const std::string& message) {
    if (static_cast<int>(level) < g_level) {
        return;
    }
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::steady_clock::now() - g_start).count();
    size_t tid = std::hash<std::thread::id>{}(std::this_thread::get_id()) % 10000;
    std::lock_guard<std::mutex> lock(g_mu);
    std::fprintf(stderr, "[%7lldms] %s t%04zu %-10s %s\n", static_cast<long long>(ms), name(level),
                 tid, component.c_str(), message.c_str());
}

}  // namespace logging
