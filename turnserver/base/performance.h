// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
//
// utilities for getting the cpu usage of the caller process

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>

namespace agora {
namespace base {

struct CpuTimeData {
  CpuTimeData() : user_time(0), sys_time(0), prev_ts(0) {}

  long user_time;  // NOLINT(runtime/int)
  long sys_time;  // NOLINT(runtime/int)
  long prev_ts;  // NOLINT(runtime/int)
};

// Returns the cpu usage of self process in percentage
double GetSelfCpuUsage(CpuTimeData *prev);

// NOTE(liuyong): DO NOT use/operate the following structure. It is
// intentionally designed to internal use
struct CpuTimeDataPerThread {
  CpuTimeDataPerThread();

  int64_t user_time;
  int64_t sys_time;
  int64_t tick_in_ms;
};

inline CpuTimeDataPerThread::CpuTimeDataPerThread() {
  user_time = 0;
  sys_time = 0;
  tick_in_ms = 0;
}

double GetCpuUsageInCurrentThread(CpuTimeDataPerThread *opaque_data);

struct SystemCpuTimeData {
  SystemCpuTimeData();

  unsigned long long user_time;  // NOLINT(runtime/int)
  unsigned long long niced_time;  // NOLINT(runtime/int)
  unsigned long long system_time;  // NOLINT(runtime/int)
  unsigned long long idle_time;  // NOLINT(runtime/int)
  unsigned long long iowait_time;  // NOLINT(runtime/int)
  unsigned long long irq_time;  // NOLINT(runtime/int)
  unsigned long long softirq_time;  // NOLINT(runtime/int)
  long long prev_ms;  // NOLINT(runtime/int)
};

inline SystemCpuTimeData::SystemCpuTimeData() {
  user_time = 0;
  niced_time = 0;
  system_time = 0;
  idle_time = 0;
  iowait_time = 0;
  irq_time = 0;
  softirq_time = 0;
  prev_ms = 0;
}

double GetSystemCpuUsage(SystemCpuTimeData *prev);

}  // namespace base
}  // namespace agora
