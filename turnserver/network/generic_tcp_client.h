// Copyright (c) 2014-2017 Agora.io, Inc.
// TODO(liuyong): Gives a detailed description
//

#pragma once  // NOLINT(build/header_guard)

#include <arpa/inet.h>

#include <cstdint>

#include "base/timer.h"

#include "network/address_types.h"
#include "network/generic_tcp_connection.h"

namespace agora {
namespace base {
class Unpacker;
class EventLoop;
}  // namespace base

namespace network {

class GenericTcpClient
  : private GenericTcpConnection::Visitor {
 public:
  class Visitor {
   public:
    virtual void OnConnect(GenericTcpClient *conn, bool connected) = 0;
    virtual void OnDataReceived(GenericTcpClient *conn,
                                const void *buffer, uint32_t length) = 0;
    virtual void OnSocketError(GenericTcpClient *conn) = 0;
   protected:
    virtual ~Visitor() = default;
  };

  // (ip, port) should both be host-ordered
  GenericTcpClient(
      base::EventLoop *loop, Visitor *visitor,
      uint32_t remote_ip, uint16_t remote_port, uint16_t local_port = 0,
      GenericTcpConnection::LogMode mode = GenericTcpConnection::kVerbose);

  GenericTcpClient(
      base::EventLoop *loop, Visitor *visitor,
      const SocketAddress4 &remote_address, uint16_t local_port = 0,
      GenericTcpConnection::LogMode mode = GenericTcpConnection::kVerbose);

  GenericTcpClient(
      base::EventLoop *loop, Visitor *visitor,
      const GeneralSocketAddress &remote_address, uint16_t local_port = 0,
      GenericTcpConnection::LogMode mode = GenericTcpConnection::kVerbose);

  virtual ~GenericTcpClient();

  void RegisterCallback(Visitor *visitor);

  bool SendData(const void *buffer, uint32_t length);

  bool EnableMptcp(bool enable);

  int Detach();

  void Flush();

  void Close();

  bool Connect();

  bool IsConnected() const;
  network::SocketAddress4 GetRemoteAddress() const;
  network::SocketAddress4 GetLocalAddress() const;

 private:
  void OnConnect(int events);

  bool TurnOnMptcp(int fd);

  // Inherited from |GenericTcpConnection::Visitor|
  virtual void OnError(GenericTcpConnection *conn, int events) OVERRIDE;
  virtual void OnDataReceived(GenericTcpConnection *conn,
                              const void *buffer, uint32_t length) OVERRIDE;

 private:
  static void ConnectCallback(
      bufferevent *bev, short events,  // NOLINT(runtime/int)
      void *context);

 private:
  base::EventLoop *loop_;
  Visitor *visitor_;

  GenericTcpConnection::LogMode mode_;

  network::SocketAddress4 remote_address_;
  uint16_t local_port_;

  bufferevent *pending_;
  GenericTcpConnection *conn_;

  bool connecting_;
  bool stopped_;

  bool mptcp_enabled_;
};

}  // namespace network
}  // namespace agora
