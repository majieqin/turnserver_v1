// Copyright (c) 2014-2017 Agora.io, Inc.
// A class helping to communnicate with multiple eslb servers,
// used by |voserver| and |dispatcher|
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <memory>
#include <vector>

#include "media_server_library/network/address_types.h"
#include "media_server_library/base/config.h"
#include "media_server_library/base/cpp_feature.h"

#include "media_server_library/network/tcp_client.h"

namespace agora {
namespace network {

class EslbClientListener {
 public:
  virtual void OnEslbResponsed(const network::SocketAddress4 &addr) = 0;

 protected:
  virtual ~EslbClientListener() {}
};

class EslbClient : private base::NonCopyOrMovable, private TcpClientCallback {
 public:
  // |ips| should be host-ordered
  EslbClient(base::EventLoop *loop, const std::vector<uint32_t> &ips,
             uint16_t port, EslbClientListener *listener);

  virtual ~EslbClient();

  void Connect();

  void UpdateEslbAddresses(
      const std::vector<network::SocketAddress4> &addresses);

  void AllocateEdgeService(const network::SocketAddress4 &previous);

 private:
  typedef std::unique_ptr<TcpClient> TcpClientPtr;
  typedef std::vector<TcpClientPtr> TcpClientList;

 private:
  virtual void OnConnect(TcpClient *c, bool connected) OVERRIDE;
  virtual void OnPacketReceived(
      TcpClient *c, base::Unpacker *p, uint16_t server_type,
      uint16_t uri) OVERRIDE;

  virtual void OnSocketError(TcpClient *client) OVERRIDE;
  virtual void TimeToPing(TcpClient *client) OVERRIDE;

 private:
  void OnRejoinResponsed(
      TcpClient *conn, uint32_t code, const network::SocketAddress4 &addr);

 private:
  base::EventLoop *loop_;

  network::SocketAddress4 prev_addr_;

  TcpClientList eslbs_;
  EslbClientListener *listener_;
};

}  // namespace network
}  // namespace agora
