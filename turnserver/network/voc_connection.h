// Copyright (c) 2014-2017 Agora.io, Inc.
// A class helping to communnicating with a single voc service,
// for allocating a unique identifier.

#pragma once  // NOLINT(build/header_guard)

#include <netinet/in.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "network/address_types.h"
#include "base/cpp_feature.h"
#include "base/event_loop.h"
#include "base/packet.h"

#include "network/tcp_client.h"

#include "protocol/voc_protocol.h"

namespace agora {
namespace network {

class VocConnection;

// ServiceId --> ServerInfo2
using protocol::voc::ServiceInfo;
using protocol::voc::ServiceInfoList;

typedef std::unordered_map<uint32_t, ServiceInfo> ServiceInfoMap;
typedef std::unordered_map<std::string, ServiceInfoMap> InterestedServiceMap;

// |ip| is host-ordered
struct VocConnectionCallback {
  virtual void OnSocketError(VocConnection *from) = 0;

  virtual void OnJoinReplied(
      VocConnection *from, uint32_t code, uint32_t service_id, uint32_t ip,
      const base::ConfigMap &config) = 0;

  virtual void OnAllServiceInfoReceived(
      VocConnection *from, const std::string &path,
      const ServiceInfoList &all) = 0;

  virtual void OnDeltaServiceInfoReceived(
      VocConnection *from, const std::string &path,
      const std::vector<ServiceInfo> &in,
      const std::vector<ServiceInfo> &gone) = 0;

 protected:
  virtual ~VocConnectionCallback() {}
};

class VocConnection :
  private TcpClientCallback,
  private base::NonCopyOrMovable {
 public:
  // |ip, port| should be host-ordered
  VocConnection(event_base *base, uint32_t ip, uint16_t port,
                uint16_t service_port, const std::string &path,
                VocConnectionCallback *callback, uint32_t local_ip = 0);

  virtual ~VocConnection();

  const InterestedServiceMap& GetAllLiveServices() const;
  network::SocketAddress4 GetRemoteAddress() const;

  void Connect();
  void Stop();

  void SetServicePort(uint16_t port);
  bool SendPacket(const base::Packet &p);

  void UpdateServiceId(uint32_t id);
  void SubscribeService(const std::string &path);
  void UnsubscribeService(const std::string &path);

 private:
  virtual void OnConnect(TcpClient *conn, bool connected) OVERRIDE;
  virtual void OnPacketReceived(TcpClient *conn, base::Unpacker *pkr,
                                uint16_t service_type, uint16_t uri) OVERRIDE;

  virtual void OnSocketError(TcpClient *conn) OVERRIDE;
  virtual void TimeToPing(TcpClient *conn) OVERRIDE;

  void SendJoinRequest();
  void SendSubscriptionPacket(const std::string &path, bool is_add);

  void HandleJoinResponse(TcpClient *conn, base::Unpacker *pkr);
  void HandlePong(TcpClient *c, base::Unpacker *pkr);
  void HandleFullServiceInfo(TcpClient *c, base::Unpacker *p);
  void HandlePartialServiceInfo(TcpClient *c, base::Unpacker *p);

  void UpdateAllServers(const std::string &path,
                        const std::vector<protocol::voc::ServiceInfo> &all);

  void UpdateDeltaServers(
      const std::string &path,
      const std::vector<protocol::voc::ServiceInfo> &in,
      const std::vector<protocol::voc::ServiceInfo> &gone);

 private:
  uint32_t my_id_;
  uint32_t local_ip_;
  uint16_t service_port_;  // udp port for other node to connect
  std::string my_path_;

  event_base *base_;
  network::SocketAddress4 remote_;

  std::unique_ptr<TcpClient> client_;
  VocConnectionCallback *callback_;

  InterestedServiceMap live_services_;
};

inline const InterestedServiceMap& VocConnection::GetAllLiveServices() const {
  return live_services_;
}

inline network::SocketAddress4 VocConnection::GetRemoteAddress() const {
  assert(client_ != nullptr);

  return client_->GetRemoteAddress2();
}

inline void VocConnection::SetServicePort(uint16_t port) {
  service_port_ = port;
}

}  // namespace network
}  // namespace agora
