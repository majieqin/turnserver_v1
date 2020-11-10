// Copyright (c) 2014-2017 Agora.io, Inc.
// A safe c++ logger to print formatted logs to syslog service
//

#pragma once  // NOLINT(build/header_guard)

#include "log.h"

#ifdef LOGGING_FORCE_SYSLOG

#include <sstream>

namespace agora {
namespace logging {

class SafeLogger {
 public:
  SafeLogger(const char *file, int line, Severity severity);
  ~SafeLogger();

  std::ostream& stream();

 private:
  Severity severity_;
  std::ostringstream sout_;
};

inline SafeLogger::SafeLogger(const char *file, int line, Severity severity) {
  severity_ = severity;
  sout_ << file << ":" << line << ": ";
}

inline SafeLogger::~SafeLogger() {
  Log(severity_, "%s", sout_.str().c_str());
}

inline std::ostream& SafeLogger::stream() {
  return sout_;
}

}  // namespace logging
}  // namespace agora

#define SAFE_LOG2(level) agora::logging::IsLoggingEnabled( \
    agora::logging::k##level##Log) && agora::logging::SafeLogger( \
    __FILE__, __LINE__, agora::logging::k##level##Log).stream()

#define SAFE_LOG_IF2(cond, level) (cond) && agora::logging::IsLoggingEnabled( \
    agora::logging::k##level##Log) && agora::logging::SafeLogger( \
    __FILE__, __LINE__, agora::logging::k##level##Log).stream()

#endif
