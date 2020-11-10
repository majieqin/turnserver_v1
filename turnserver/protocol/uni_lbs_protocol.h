// Copyright (c) 2014-2017 Agora.io, Inc.
// Unified LBS protocol
//

#pragma once  // NOLINT(build/header_guard)

#include <string>
#include <vector>

#include "packet.h"
#include "packet_helper.h"

namespace agora {
namespace protocol {
namespace lbs {

enum TicketVersion {
  // V2 = 2,
  // V3 = 3,
  // V4 = 4,
  kVersion5 = 5,
  // V6 = 6,  // NOTE(liuyong): For hacking...., obsolscent, damn it
  kVersion7 = 7,  // NOTE(liuyong): For hacking now, no ticket required
  kVersion8 = 8,
};

NEW_DECLARE_PACKABLE_2(AccessTicket, std::vector<uint8_t>, content,
                       std::vector<uint8_t>, signature);

NEW_DECLARE_PACKET_6(TicketDetail5, kVersion5, uint32_t, vid, uint32_t, uid,
                     std::vector<uint32_t>, sids, uint32_t, issue_ts, uint32_t,
                     due_ts, std::string, cname);

NEW_DECLARE_PACKABLE_7(DetailedSignature5, uint32_t, cid, uint32_t, uid,
                       uint32_t, vid, std::vector<uint32_t>, sids, uint32_t,
                       issue_ts, uint32_t, due_ts, std::string, cname);

NEW_DECLARE_PACKET_5(TicketDetail7, kVersion7, uint32_t, vid, uint32_t, uid,
                     uint32_t, issue_ts, uint32_t, due_ts, std::string, cname);



NEW_DECLARE_PACKET_8(TicketDetail8, kVersion8,
                     uint32_t, vid, uint32_t, uid, uint32_t, cid,
                     std::vector<uint32_t>, sids, uint32_t, issue_ts, uint32_t,
                     due_ts, std::string, cname, uint32_t, bitrates);

enum ServiceUri {
  kLbsTestJoinReqUri = 1000,
  kLbsJoinReqUri = 1001,
  kLbsJoinResUri = 1002,
  kLbsPingUri = 1003,
  kLbsPongUri = 1004,
  kLbsFullChannelListUri = 1005,
  kLbsDeltaChannelListUri = 1006,
  kLbsJoinReq2Uri = 1007,
  kLbsJoinRes2Uri = 1008,
};
typedef std::map<uint32_t, uint32_t> DetailInfoMap;
typedef std::map<uint32_t, std::string> DetailInfo2Map;
enum {
  kUserLoad = 1,
  kCpuLoad = 2,
  kTrafficInLoad = 3,
  kTrafficOutLoad = 4,
  kUserLoadMax = 5,
  kServerVersion = 6,
  kServerDetail = 7,
  kMemLoad = 8,
};

NEW_DECLARE_PACKET_4(LbsTestJoinReq, kLbsTestJoinReqUri, uint32_t, id,
                     std::string, service_name, std::string, ip, uint16_t,
                     port);

NEW_DECLARE_PACKET_3(LbsJoinReq, kLbsJoinReqUri, uint32_t, id, std::string,
                     service_name, uint16_t, port);

NEW_DECLARE_PACKET_2(LbsJoinRes, kLbsJoinResUri, uint32_t, id, uint32_t, code);

NEW_DECLARE_PACKET_4(LbsJoinReq2, kLbsJoinReq2Uri, uint32_t, id, std::string,
                     service_name, uint16_t, port, DetailInfo2Map, detail);

NEW_DECLARE_PACKET_3(LbsJoinRes2, kLbsJoinRes2Uri, uint32_t, id,
    uint32_t, code, DetailInfo2Map, detail);


NEW_DECLARE_PACKET_2(LbsPing, kLbsPingUri, uint32_t, id, DetailInfoMap,
                     load_detail)
NEW_DECLARE_PACKET_2(LbsPong, kLbsPongUri, uint32_t, id, DetailInfoMap,
                     load_detail);

typedef std::vector<std::pair<uint32_t, std::string>> AllChannelVector;

NEW_DECLARE_PACKET_2(LbsFullChannelList, kLbsFullChannelListUri, uint32_t, id,
                     AllChannelVector, channels);

NEW_DECLARE_PACKET_3(LbsDeltaChannelList, kLbsDeltaChannelListUri, uint32_t, id,
                     AllChannelVector, added, AllChannelVector, removed);

}  // namespace lbs
}  // namespace protocol
}  // namespace agora
