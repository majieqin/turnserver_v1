// Copyright (c) 2014-2017 Agora.io, Inc.
// Protocol for ELBS(Edge Server Load Balancer)
// used by |voserver|, and |dispatcher/lbes|
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <string>
#include <vector>

#include "packet_helper.h"

namespace agora {

namespace commons {

enum ElbsUriType {
  kRejoinRequestUri = 6,
  kRejoinResponseUri = 7,

  kRejoinRequest2Uri = 8,
  kRejoinResponse2Uri = 9,

  kUapUri = 1000,
};

enum ErrorCode {
  //NOTE: [0-10000] is for shared use
  kOk = 0,
  kFail = 1,
  kTimestampExpired = 2,

  kServiceNotReady = 9,
  kServiceTooHeavy = 10,

  kNoNeedToChange = 13,

  kRequestPending = 65535,
};

enum { 
  ELBS_SERVER_TYPE = 305,
  UAP_SERVER_TYPE = 1000,
};
const uint16_t kUapServiceType = 1000;

NEW_DECLARE_PACKET_1(WorkerManagerComm, kUapUri, std::string, json);

// |ip| be network-ordered, |port| host-ordered
NEW_DECLARE_PACKET_2(RejoinRequestProtocol, kRejoinRequestUri,
                     uint32_t, ip, uint16_t, port);

// |ip| be network-ordered, |port| host-ordered
NEW_DECLARE_PACKET_3(RejoinResponseProtocol, kRejoinResponseUri,
                     uint32_t, code, uint32_t, ip, uint16_t, port);

// |ip|, |port| should be both host-ordered.
NEW_DECLARE_PACKET_2(RejoinRequest2Protocol, kRejoinRequest2Uri,
                     uint32_t, ip, uint16_t, port);

// |ip|, |port| should be both host-ordered.
NEW_DECLARE_PACKET_3(RejoinResponse2Protocol, kRejoinResponse2Uri,
                     uint32_t, code, uint32_t, ip, uint16_t, port);

}}  // namespace agora::commons
