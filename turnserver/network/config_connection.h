// Copyright (c) 2014-2017 Agora.io, Inc.
// A class helping to communnicating with a single ConfigServer
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "network/address_types.h"
#include "base/cpp_feature.h"

#include "network/tcp_client.h"

#include "network/protocol/config_server_protocol.h"

struct event_base;

namespace agora {
namespace base {
class Packet;
class Unpacker;
}  // namespace base

namespace network {

class ConfigConnection;

struct ConfigConnectionCallback {
  virtual void OnConfigLoaded(ConfigConnection *from, const std::string &config,
                              uint32_t version) = 0;
  virtual void OnWildMatchConfigLoaded(ConfigConnection *from,
                    const protocol::config_server::ConfigInfoMap &configs,
                    const std::string &my_idc) = 0;

 protected:
  virtual ~ConfigConnectionCallback() {}
};

class ConfigConnection : private TcpClientCallback {
 public:
  ConfigConnection(
      event_base *loop, const network::SocketAddress4 &remote,
      const std::string &service_name, ConfigConnectionCallback *callback,
      bool enable_wild_match = false);

  virtual ~ConfigConnection();

  uint32_t my_id() const;
  network::SocketAddress4 GetRemoteAddress() const;
  bool IsConnected() const;

  void UpdateVersion(uint32_t version);

  void RequestLatestConfig(
      uint32_t service_id, const std::string &idc, uint32_t current_version);
  void RequestLatestConfig(uint32_t service_id,
      const protocol::config_server::ConfigInfoMap& config);

 private:
  virtual void OnConnect(TcpClient *conn, bool connected) OVERRIDE;
  virtual void OnPacketReceived(
      TcpClient *conn, base::Unpacker *pkr, uint16_t service_type,
      uint16_t uri) OVERRIDE;

  virtual void OnSocketError(TcpClient *conn) OVERRIDE;
  virtual void TimeToPing(TcpClient *conn) OVERRIDE;

 private:
  // once |service_id| is set, it is ready to connect to the ConfigServer
  void Connect();
  void OnConfigResponse(TcpClient *conn, base::Unpacker *p);
  void OnWildMatchConfigResponse(TcpClient *client, base::Unpacker *p);
  bool SendPacket(const base::Packet &p);

 private:
  bool joined_;
  int32_t last_ping_ts_;

  event_base *loop_;
  network::SocketAddress4 remote_;
  uint32_t my_id_;
  uint16_t local_port_;

  std::string idc_;
  std::string service_name_;

  uint32_t version_;

  ConfigConnectionCallback *callback_;
  std::unique_ptr<TcpClient> c_;

  protocol::config_server::ConfigInfoMap local_configs_;

  bool enable_wild_match_;
};

inline uint32_t ConfigConnection::my_id() const {
  return my_id_;
}

inline network::SocketAddress4 ConfigConnection::GetRemoteAddress() const {
  return remote_;
}

inline bool ConfigConnection::IsConnected() const {
  return joined_;
}

inline void ConfigConnection::UpdateVersion(uint32_t version) {
  version_ = version;
}

}  // namespace network
}  // namespace agora
