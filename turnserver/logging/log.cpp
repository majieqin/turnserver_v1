// Copyright (c) 2018, Agora.io, Inc.
//


#include "logging/log.h"

#ifdef LOGGING_FORCE_SYSLOG
#include "logging/syslog_log.cpp"
#else
#include "logging/glog_log.cpp"
#endif

namespace agora {
namespace logging {

LogMode FLAGS_log_mode = LogMode::kSilent;

}  // namespace logging
}  // namespace agora
