// Copyright 2014-2017 Agora.io
// Author: yurun
// Mail: sunyurun@agora.io
// Created Time: 2017-03-16 13:01:51
// This is a plug-in statistics tool to accumulate packet counter and bytes
// by uri.
// 1. Output in json format:
// {
//    "ts":1470987198752304,
//    "sent":[
//      {"uri":28,"count":9,"bytes":11430},
//      {"uri":29,"count":3,"bytes":210},
//      {"uri":41,"count":9,"bytes":450}
//    ],
//    "received":[
//      {"uri":100,"count":3,"bytes":3753},
//      {"uri":101,"count":6,"bytes":1506}
//    ]
// }
// 2. Output in txt format:
// [ts]
// 1470987198752304
// [sent]
// 0 28 9 11430
// 0 29 3 210
// 0 41 9 450
// [received]
// 0 100 3 3753
// 0 101 6 1506

#pragma once  // NOLINT(build/header_guard)

#include <string>
#include <unordered_map>

namespace agora {
namespace network {

class PacketCounterTool {
 public:
  PacketCounterTool();
  ~PacketCounterTool();

  void SetLocalAddress(const std::string &address);

  void AddSentPacket(uint16_t type, uint16_t uri, uint32_t bytes);
  void AddReceivedPacket(uint16_t type, uint16_t uri, uint32_t bytes);

  std::string OutputJsonString(int64_t ts) const;
  std::string OutputTextString(int64_t ts) const;

 private:
  struct PacketCounter {
    PacketCounter() : count(0), bytes(0) {}

    uint64_t count;
    uint64_t bytes;
  };

  std::string local_address_;
  std::unordered_map<uint32_t, PacketCounter> sent_counters_;
  std::unordered_map<uint32_t, PacketCounter> received_counters_;
};

class VendorCounterTool {
 public:
  enum Direction {
    kToVos = 0,
    kToRouter,
    kToSdk,
    kFromVos,
    kFromRouter,
    kFromSdk,
    kDirectionCount,
  };

  VendorCounterTool();
  ~VendorCounterTool();

  bool AddWatchedVendor(uint32_t vid);
  bool DelWatchedVendor(uint32_t vid);

  void AddPacket(uint32_t vid, Direction direction, uint64_t bytes);

  std::string OutputJsonString(uint64_t ts) const;

 private:
  struct Counter {
    uint64_t count;
    uint64_t bytes;
  };

  struct Counters {
    Counter counters[static_cast<size_t>(Direction::kDirectionCount)];
  };

  typedef std::unordered_map<uint32_t, Counters> VendorCounter;

  static std::string OutputDirectionCounter(const Counters &dc);

 private:
  VendorCounter vendor_counters_;
};

}  // namespace network
}  // namespace agora
