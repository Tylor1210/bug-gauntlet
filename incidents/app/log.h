// log.h: tiny thread-safe leveled logger used by every component of linkpage-sim.
#pragma once

#include <string>

enum class LogLevel { Debug = 0, Info = 1, Warn = 2, Error = 3 };

namespace logging {

void set_level(LogLevel level);
void write(LogLevel level, const std::string& component, const std::string& message);

inline void debug(const std::string& c, const std::string& m) { write(LogLevel::Debug, c, m); }
inline void info(const std::string& c, const std::string& m) { write(LogLevel::Info, c, m); }
inline void warn(const std::string& c, const std::string& m) { write(LogLevel::Warn, c, m); }
inline void error(const std::string& c, const std::string& m) { write(LogLevel::Error, c, m); }

}  // namespace logging
