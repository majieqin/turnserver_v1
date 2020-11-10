// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
// base logging utilities
// TODO(liuyong): A more detailed description is required.

#pragma once  // NOLINT(build/header_guard)

#include <syslog.h>

#include <glog/logging.h>

#include <cassert>
#include <cstdarg>
#include <cstdint>
#include <string>
#include <functional>
#include <cstdio>

#include "../base/reflection.hpp"

namespace agora {
namespace logging {

// Compile time debug logs, define macro NDEBUG

struct LoggingGuard {
  /// \brief LoggingGuard Construct
  ///
  /// Mark a guard as logging enabled. Use default glog settings.
  /// Log Levels:
  ///   DEBUG, INFO, WARN, ERROR, FATAL (from 0 to 4)
  /// Default settings:
  ///   - v: 4
  ///   - log_dir: /var/log/voice/{service_name}
  ///   - logbuflevel: 2 (Warning)
  ///   - max_log_size: 100 MB
  ///   - minloglevel: 1 (Info)
  /// If no daemon, log to stderr instead.
  explicit LoggingGuard(void(*overload)() = nullptr);
  ~LoggingGuard();

  LoggingGuard(const LoggingGuard&) = delete;
  LoggingGuard(LoggingGuard&&) = delete;
  LoggingGuard& operator=(const LoggingGuard& rhs) = delete;

 private:
  void ReadFlagsFromFile();
};

void SetGlobalVerboseLevel(int lvl);
void SetModuleVerboseLevel(const char* module, int lvl);

using LogGuard = LoggingGuard;

void EnableDebugLogging(bool enabled);

namespace detail {
std::string StringFormat(const std::string fmt, ...);
}  // namespace detail

}  // namespace logging
}  // namespace agora

// severity name backward compatibility
#define FIX_SEVERTIY(severity) FIX_SEVERTIY_##severity
#define FIX_SEVERTIY_Debug RDEBUG
#define FIX_SEVERTIY_Info INFO
#define FIX_SEVERTIY_Warn WARNING
#define FIX_SEVERTIY_Error ERROR
#define FIX_SEVERTIY_Fatal FATAL
#define FIX_SEVERTIY_Notice WARNING

DECLARE_bool(daemon);
// namespace fLB {
//  extern GOOGLE_GLOG_DLL_DECL type FLAGS_daemon;
// }
// using fLB::FLAGS_daemon;
// DECLARE_VARIABLE(bool, B, daemon, bool);

#define GLOG_LEVEL(severity) \
  __MACRO_CONCAT(google::GLOG, __MARCO_EXPAND(FIX_SEVERTIY(severity)))

#define LOG_IS_ON(severity) \
  (__MARCO_EXPAND(GLOG_LEVEL(severity)) >= FLAGS_minloglevel)

// Backward compatibility

#define SAFE_LOG2(severity) \
  LOG_IF(__MARCO_EXPAND(FIX_SEVERTIY(severity)), \
      __MARCO_EXPAND(GLOG_LEVEL(severity)) >= FLAGS_minloglevel)

#define SAFE_LOG_IF2(cond, severity) \
  LOG_IF(__MARCO_EXPAND(FIX_SEVERTIY(severity)), cond)

#define DO_LOG2(...) LOG2(__VA_ARGS__)

#define LOG_FAST2 LOG2

#define LOG2(severity, ...) SAFE_LOG2(severity) \
  << ::agora::logging::detail::StringFormat(__VA_ARGS__)

#define LOG_EVERY_N2(severity, N, ...) LOG_EVERY_N(FIX_SEVERTIY(severity), N) \
  << ::agora::logging::detail::StringFormat(__VA_ARGS__)

#define LOG_IF2(severity, cond, ...) SAFE_LOG_IF2(cond, severity) \
  << ::agora::logging::detail::StringFormat(__VA_ARGS__)

#ifdef DCHECK
#undef DCHECK
#endif

#define DCHECK(cond) CHECK(cond)


// New Features:
#define SAFE_LOG_EVERY_N2(severity, N) \
  LOG_EVERY_N(__MARCO_EXPAND(FIX_SEVERTIY(severity)), N)

#define SAFE_ASSERT2(cond) LOG_ASSERT(cond)

#define SAFE_LOG_STRING2(severity, outvec) \
  LOG_STRING(__MARCO_EXPAND(FIX_SEVERTIY(severity)), outvec)

#define SAFE_PLOG2(severity) PLOG(__MARCO_EXPAND(FIX_SEVERTIY(severity)))

#define SAFE_VLOG2 VLOG
#define SAFE_VLOG_IF2 VLOG_IF
#define SAFE_VLOG_EVERY_N2 VLOG_EVERY_N
#define SAFE_LOG_IF_EVERY_N2 VLOG_IF_EVERY_N

