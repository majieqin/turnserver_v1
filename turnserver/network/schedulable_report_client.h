// Copyright (c) 2014-2017 Agora.io, Inc.
// TODO(liuyong): requires a detailed description
//

#pragma once  // NOLINT(build/header_guard)

#include <google/protobuf/message.h>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "msgpack.hpp"

#include "media_server_library/network/address_types.h"
#include "media_server_library/base/cpp_feature.h"
#include "media_server_library/base/timer.h"

#include "media_server_library/network/eslb_client.h"
#include "media_server_library/network/report_client.h"
#include "media_server_library/thrift/protocol/TCompactProtocol.h"
#include "media_server_library/thrift/transport/TBufferTransports.h"

namespace agora {
namespace network {

class SchedulableReportClient : private EslbClientListener,
                                private ReportClientCallback,
                                private base::NonCopyOrMovable {
 public:
  SchedulableReportClient(
      base::EventLoop *base, const network::SocketAddress4 &edge,
      int32_t refresh_interval = 60);  // seconds

  SchedulableReportClient(
      base::EventLoop *base, const network::NewSockAddress4 &edge,
      int32_t refresh_interval = 60);  // seconds

  virtual ~SchedulableReportClient();

  void UpdateLbsAddresses(
      const std::vector<network::SocketAddress4> &addresses);

  void UpdateLbsAddresses(
      const std::vector<network::NewSockAddress4> &addresses);

  bool Initialize();

  bool SendPacket(const void *data, uint32_t length,
                  PacketType type, AckMode ack);

  template <typename T>
  bool SendMsgpackPacket(const T &packet, AckMode ack = kNoAckRequired);

  template <typename T>
  bool SendThriftPacketToDebugEnv(const T &item) {
    return SendThriftPacket(item, kAckDebug);
  }

  template <typename T>
  bool SendThriftPacket(const T &item, AckMode ack = kNoAckRequired) {
    apache::thrift::protocol::TCompactProtocolFactory protocolFactory;
    apache::thrift::transport::TTransportFactory transportFactory;
    auto trans = std::make_shared<apache::thrift::transport::TMemoryBuffer>();
    auto p = protocolFactory.getProtocol(transportFactory.getTransport(trans));
    if (item.write(p.get()) > 0) {
      p->getTransport()->flush();
      if (trans->available_read()) {
        uint8_t *data = nullptr;
        uint32_t sz = 0;
        trans->getBuffer(&data, &sz);
        if (data && sz > 0) {
          const void *data_ptr = reinterpret_cast<void *>(data);
          return SendPacket(data_ptr, sz, kThrift, ack);
        }
      }
    }

    return false;
  }

  bool SendProtocolPakcet(const google::protobuf::Message &item,
                          AckMode ack = kNoAckRequired) {
    std::string str_buff;
    item.SerializeToString(&str_buff);
    return SendPacket(str_buff.data(), str_buff.size(), kProtoBuffer, ack);
  }

 private:
  typedef std::unique_ptr<ReportClient> ReportClientPtr;

  virtual void OnEslbResponsed(const network::SocketAddress4 &addr) OVERRIDE;

  virtual void OnConnect(ReportClient *conn, bool connected) OVERRIDE;
  virtual void OnPacketReceived(
      ReportClient *conn, const void *buf, size_t length) OVERRIDE;

  virtual void OnSocketError(ReportClient *conn) OVERRIDE;
  virtual void TimeToPing(ReportClient *conn) OVERRIDE;

  bool SendPacketInternal(const void *data, uint32_t length,
                          PacketType type, AckMode ack);

  void OnTimer();

  bool SaveToCache(
      const void *data, uint32_t length, PacketType type, AckMode ack);

  void SendCachedPackets();

 private:
  base::EventLoop *base_;
  EslbClient eslb_client_;

  struct CachedPacket {
    PacketType type;
    AckMode ack_mode;
    std::vector<uint8_t> payload;
  };

  std::queue<CachedPacket> pending_packets_;

  int32_t sent_packets_;
  int32_t dropped_packets_;

  ReportClientPtr tcp_active_;
  ReportClientPtr tcp_connecting_;
  ReportClientPtr tcp_closing_;
  int32_t last_reconnect_ts_;

  base::RepeatedTimer timer_;
};

template <typename T>
bool SchedulableReportClient::SendMsgpackPacket(const T &packet, AckMode ack) {
  msgpack::sbuffer data;
  packet.pack(data);
  return SendPacket(data.data(), data.size(), kMsgPack, ack);
}

}  // namespace network
}  // namespace agora
