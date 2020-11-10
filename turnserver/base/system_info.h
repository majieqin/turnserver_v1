// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
// Utilities for querying system-wide informations.

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <map>
#include <string>

#include "cpp_feature.h"
#include "performance.h"
#include "timer.h"

struct event_base;

namespace agora {
namespace base {

class EventLoop;

int GetAvailableCpuCount();

struct SysInfo {
  // Seconds since boot
  long uptime_second;  // NOLINT(runtime/int)

  // 1, 5, and 15 minute load averages
  unsigned long average_load[3];  // NOLINT(runtime/int)

  // Total usable main memory size
  unsigned long ram_total;  // NOLINT(runtime/int)

  // Available memory size
  unsigned long ram_free;  // NOLINT(runtime/int)

  // Amount of shared memory
  unsigned long ram_shared;  // NOLINT(runtime/int)

  // Memory used by buffers
  unsigned long ram_buffer;  // NOLINT(runtime/int)

  // Total swap space size
  unsigned long swap_total;  // NOLINT(runtime/int)

  // Swap space still available
  unsigned long swap_free;  // NOLINT(runtime/int)

  // Number of current processes
  unsigned short process_number;  // NOLINT(runtime/int)
};

struct CpuInfo {
  // unit: jiffies, 1jiffies = 0.01second
  uint64_t total;
  uint64_t work;

  // 从系统启动开始累计到当前时刻，处于用户态的运行时间，不包含nice值为负进程
  uint32_t user;

  // 从系统启动开始累计到当前时刻，nice值为负的进程所占用的CPU时间
  uint32_t nice;

  // 从系统启动开始累计到当前时刻，处于核心态的运行时间
  uint32_t system;

  /** 从系统启动开始累计到当前时刻，除IO等待时间以外的其它等待时间 */
  uint32_t idle;

  /** 从系统启动开始累计到当前时刻，IO等待时间(2.5.41) */
  uint32_t iowait;

  /** 从系统启动开始累计到当前时刻，硬中断时间(2.6.0) */
  uint32_t irq;

  /** 从系统启动开始累计到当前时刻，软中断时间(2.6.0) */
  uint32_t softirq;

  /** which is the time spent in other operating systems when running in a
   * virtualized environment(2.6.11) */
  uint32_t steal;

  /** which is the time spent running a virtual CPU for guest operating systems
   * under the control of the Linux kernel(2.6.24) */
  uint32_t guest;

  /** running a niced guest */
  uint32_t guest_nice;
};

struct MemoryInfo {
  uint32_t mem_total;
  uint32_t mem_free;
  uint32_t buffers;
  uint32_t cached;
  uint32_t swap_cached;
  uint32_t swap_total;
  uint32_t swap_free;
};

struct NicInfo {
  /** receive datas */
  uint64_t receive_bytes;
  uint64_t receive_packets;
  uint64_t receive_errors;
  uint64_t receive_dropped;
  uint64_t receive_fifo_errors;
  uint64_t receive_frame;
  uint64_t receive_compressed;
  uint64_t receive_multicast;

  /** transmit datas */
  uint64_t transmit_bytes;
  uint64_t transmit_packets;
  uint64_t transmit_errors;
  uint64_t transmit_dropped;
  uint64_t transmit_fifo_errors;
  uint64_t transmit_collisions;
  uint64_t transmit_carrier;
  uint64_t transmit_compressed;
};

struct SelfMemInfo {
  uint64_t size;  // total program size, same as VmSize in /proc/[pid]/status
  uint64_t resident;  // same as VmRSS in /proc/[pid]/status
  uint64_t share;  // shared pages
  uint64_t text;  // text (code)
  uint64_t lib;  // library (unused in Linux 2.6)
  uint64_t data;  // data + stack
  uint64_t dt;  // dirty pages (unused in Linux 2.6)
};  // unit:4KB

typedef std::map<std::string, CpuInfo> CpuInfoMap;
typedef std::map<std::string, NicInfo> NicInfoMap;

bool GetSysInfo(SysInfo *info);
bool GetMemoryInfo(MemoryInfo *info);
bool GetSelfMemInfo(SelfMemInfo *info);

class SystemInfo : private NonCopyOrMovable {
 public:
  // Ten seconds
  explicit SystemInfo(event_base *loop, int32_t interval = 10 * 1000);

  // Ten seconds
  explicit SystemInfo(EventLoop *loop, int32_t interval = 10 * 1000);
  ~SystemInfo();

  bool GetCpuInfo(CpuInfo *cpuInfo);

  const CpuInfoMap& GetCpuInfoMap();

  double GetCpuUsage();
  double GetSelfCpuUsage() const;
  double GetSelfThreadCpuUsage() const;
  uint64_t GetSelfMemResidentSize() const;

  void GetNicFlowRate(uint32_t *rx_kbps, uint32_t *tx_kbps) const;

  bool GetNicInfo(const std::string &nic_name, NicInfo *nic_info);

  const NicInfoMap& GetNicInfoMap();

  uint64_t GetAllNicReceivedBytes();
  uint64_t GetAllNicTransmittedBytes();

  bool GetNicXmittedBytes(uint64_t *rx_bytes, uint64_t *tx_bytes);

 private:
  bool LoadAllCpuInfo();
  bool LoadAllNicInfo();
  void OnTimer();

 private:
  uint64_t prev_rx_bytes_;
  uint64_t prev_tx_bytes_;
  double cpu_usage_;
  double cpu_usage_in_self_thread_;

  int64_t last_report_ms_;
  uint32_t rx_kbps_;
  uint32_t tx_kbps_;
  uint64_t mem_;  // KB

  CpuInfo prev_cpu_info_;
  CpuInfoMap cpu_info_;
  NicInfoMap nic_info_;
  CpuTimeData cpu_time_data_;
  CpuTimeDataPerThread opaque_data_per_thread_;

  base::RepeatedTimer timer_;
};

}  // namespace base
}  // namespace agora
