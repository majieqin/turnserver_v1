// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
// base logging utilities
// TODO(liuyong): A more detailed description is required.

#pragma once  // NOLINT(build/header_guard)

#ifdef LOGGING_FORCE_SYSLOG
#include "syslog_log.h"
#else
#include "glog_log.h"
#endif

namespace agora {
namespace logging {
enum class LogMode {
  kSilent = 0,
  kVerbose = 1,
};

extern LogMode FLAGS_log_mode;

}  // namespace logging
}  // namespace agora
