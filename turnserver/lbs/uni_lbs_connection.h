// Copyright (c) 2014-2017 Agora.io, Inc.
// A class helping to communnicating with a single LBS server,
//

#pragma once  // NOLINT(build/header_guard)

#include <netinet/in.h>

#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "event2/event.h"

#include "../base/cpp_feature.h"
#include "../base/timer.h"
#include "../network/address_utility.h"
#include "../network/tcp_client.h"
#include "../protocol/uni_lbs_protocol.h"

namespace agora {

namespace base {
class Packet;
class Unpacker;
}  // namespace base

namespace lbs {
class UniLbsConnection;

struct ChannelInfo {
  uint32_t vid;
  uint32_t cid;
  std::string channel_name;
};

using protocol::lbs::DetailInfo2Map;

inline bool operator<(const ChannelInfo &a, const ChannelInfo &b) {
  return a.vid < b.vid ||
         ((a.vid == b.vid) && (a.channel_name < b.channel_name));
}

struct UniLbsEventCallback {
  virtual void OnLbsConnected(UniLbsConnection *c) = 0;
  virtual void OnLbsDisconnected(UniLbsConnection *c) = 0;
  virtual void OnLbsMessageReceived(
      UniLbsConnection *c, base::Unpacker *p, uint16_t uri) = 0;

  virtual void OnUpdateEdgeServerLoadQuota(
      UniLbsConnection *c, uint32_t max_users) = 0;

  virtual void OnJoinReplied(
      UniLbsConnection *c, const DetailInfo2Map& details,
      std::vector<ChannelInfo> *channels) = 0;

  virtual void OnEdgeServerPerformanceRequired(
      uint32_t *users, uint32_t *cpu_percentage, uint32_t *rx_kbps,
      uint32_t *tx_kbps) = 0;

 protected:
  virtual ~UniLbsEventCallback() {}
};

class UniLbsConnection : private network::TcpClientCallback,
                       private base::NonCopyOrMovable {
 public:
  // host-ordered |service_port|
  UniLbsConnection(
      event_base *loop, const network::NewSockAddress4 &lbs_addr,
      const std::string &service_name, uint32_t service_id,
      uint16_t service_port, const std::string &release_version,
      UniLbsEventCallback *callback);

  virtual ~UniLbsConnection();

  bool Connect();
  bool SendPacket(const base::Packet &p);

  void DoChannelCreate(ChannelInfo c);
  void DoChannelDestroy(ChannelInfo c);

 private:
  virtual void OnConnect(network::TcpClient *conn, bool connected) OVERRIDE;
  virtual void OnPacketReceived(network::TcpClient *conn, base::Unpacker *pkr,
                                uint16_t service_type, uint16_t uri) OVERRIDE;

  virtual void TimeToPing(network::TcpClient *conn) OVERRIDE;
  virtual void OnSocketError(network::TcpClient *conn) OVERRIDE;

 private:
  void OnJoinReplied(network::TcpClient *client, base::Unpacker *p);
  void OnJoin2Replied(network::TcpClient *client, base::Unpacker *p);
  void OnPong(network::TcpClient *client, base::Unpacker *p);
  void SendFullChannelList(std::vector<ChannelInfo> channels);

 private:
  bool joined_;
  uint32_t service_id_;
  uint16_t service_port_;  // host-ordered

  event_base *loop_;

  std::string service_name_;

  UniLbsEventCallback *callback_;
  network::TcpClient connection_;
  std::string release_version_;
};

}  // namespace lbs
}  // namespace agora
