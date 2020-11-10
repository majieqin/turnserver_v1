// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
// base logging utilities

#include "glog_log.h"

#include <glog/logging.h>
#include <glog/vlog_is_on.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "server_id.h"
#include "string_utility.h"

DEFINE_bool(daemon, true, "if programm run in daemon mode");

namespace agora {
namespace logging {

void ReadGlobalProcNames() {
  auto pid = getpid();
  std::ostringstream os;
  os << "/proc/" << pid << "/cmdline";

  std::ifstream is(os.str());

  std::string line;
  std::getline(is, line);

  std::vector<std::string> elems;
  std::stringstream ss(line);
  std::string item;

  while (std::getline(ss, item, '\0')) {
    elems.push_back(item);
  }

  std::vector<char const*> args;
  for (uint32_t i = 0; i < elems.size(); i++) {
    args.push_back(elems[i].c_str());
  }

  google::SetArgv(elems.size(), const_cast<const char**>(args.data()));
}

LoggingGuard::LoggingGuard(void (*overload)()) {
  ReadGlobalProcNames();

  if (FLAGS_daemon) {
    FLAGS_logtostderr = 0;
    FLAGS_alsologtostderr = 0;
  } else {
    FLAGS_logtostderr = 1;
    FLAGS_colorlogtostderr = 1;
    FLAGS_stderrthreshold = 1;
  }

  FLAGS_minloglevel = 1;
  FLAGS_logbuflevel = 3;
  FLAGS_v = 0;
  FLAGS_max_log_size = 3;

  FLAGS_stop_logging_if_full_disk = true;
  FLAGS_log_dir = "/var/log/agora";

  ReadFlagsFromFile();

  if (overload != nullptr) {
    overload();
  }

  google::InitGoogleLogging(google::GetArgv0());

  google::InstallFailureSignalHandler();
}

LoggingGuard::~LoggingGuard() { google::ShutdownGoogleLogging(); }

void LoggingGuard::ReadFlagsFromFile() {
  struct stat buffer;
  if (stat(GLOBAL_FLAGS_FILE, &buffer) == 0) {
    google::ReadFromFlagsFile(GLOBAL_FLAGS_FILE,
                              google::ProgramInvocationShortName(), true);
  }
  std::string special_filename = GLOBAL_FLAGS_DIR;
  special_filename += google::ProgramInvocationShortName();
  special_filename += FLAG_FILE_SUFFIX;

  if (stat(special_filename.c_str(), &buffer) == 0) {
    google::ReadFromFlagsFile(special_filename.c_str(),
                              google::ProgramInvocationShortName(), true);
  }
}

void SetGlobalVerboseLevel(int lvl) { FLAGS_v = lvl; }

void SetModuleVerboseLevel(const char* module, int lvl) {
  google::SetVLOGLevel(module, lvl);
}

void EnableDebugLogging(bool enabled) {
  if (enabled) {
    FLAGS_minloglevel = 0;
  } else {
    FLAGS_minloglevel = 1;
  }
}

namespace detail {
std::string StringFormat(const std::string fmt, ...) {
  static const int MAGIC_LENGH = 50;
  int size = fmt.size() * 2 + MAGIC_LENGH;
  std::string str;

  va_list ap;
  while (true) {
    str.resize(size);
    va_start(ap, fmt);
    int n = vsnprintf(const_cast<char*>(str.data()), size, fmt.c_str(), ap);
    va_end(ap);
    if (n > -1 && n < size) {
      str.resize(n);
      return str;
    }
    if (n > -1) {
      size = n + 1;
    } else {
      size *= 2;
    }
  }
  return str;
}
}  // namespace detail

}  // namespace logging
}  // namespace agora
