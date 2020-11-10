// Copyright (c) 2014-2017 Agora.io, Inc.
// TODO(liuyong): Gives a detailed description
//

#pragma once  // NOLINT(build/header_guard)

#include <arpa/inet.h>

#include <cstdint>

#include "timer.h"
#include "address_types.h"
#include "tcp_connection.h"

namespace agora {
namespace base {
class EventLoop;
class Unpacker;
}  // namespace base

namespace testing {
class TcpClientTester;
}  // namespace testing

namespace network {

class TcpClient;

struct TcpClientCallback {
  virtual void OnConnect(TcpClient *conn, bool connected) = 0;
  virtual void OnPacketReceived(TcpClient *conn, base::Unpacker *pkr,
                                uint16_t service_type, uint16_t uri) = 0;

  virtual void OnSocketError(TcpClient *conn) = 0;
  virtual void TimeToPing(TcpClient *conn) = 0;

 protected:
  virtual ~TcpClientCallback() {}
};

class TcpClient : private TcpConnectionCallback {
 public:
  // (ip, port) should both be host-ordered
  TcpClient(base::EventLoop *loop, uint32_t ip, uint16_t port,
            TcpClientCallback *callback, bool keep_alive = true,
            uint16_t local_port = 0,
            TcpConnection::LogMode mode = TcpConnection::kVerbose);

  TcpClient(base::EventLoop *loop, const sockaddr_in &remote,
            TcpClientCallback *callback, bool keep_alive = true,
            uint16_t local_port = 0,
            TcpConnection::LogMode mode = TcpConnection::kVerbose);

  TcpClient(base::EventLoop *loop, const SocketAddress4 &remote,
            TcpClientCallback *callback, bool keep_alive = true,
            uint16_t local_port = 0,
            TcpConnection::LogMode mode = TcpConnection::kVerbose);

  TcpClient(base::EventLoop *loop, const GeneralSocketAddress &remote,
            TcpClientCallback *callback, bool keep_alive = true,
            uint16_t local_port = 0,
            TcpConnection::LogMode mode = TcpConnection::kVerbose);

  TcpClient(event_base *base, uint32_t ip, uint16_t port,
            TcpClientCallback *callback, bool keep_alive = true,
            uint16_t local_port = 0,
            TcpConnection::LogMode mode = TcpConnection::kVerbose);

  TcpClient(event_base *base, const sockaddr_in &remote,
            TcpClientCallback *callback, bool keep_alive = true,
            uint16_t local_port = 0,
            TcpConnection::LogMode mode = TcpConnection::kVerbose);

  TcpClient(event_base *base, const SocketAddress4 &remote,
            TcpClientCallback *callback, bool keep_alive = true,
            uint16_t local_port = 0,
            TcpConnection::LogMode mode = TcpConnection::kVerbose);

  TcpClient(event_base *base, const GeneralSocketAddress &remote,
            TcpClientCallback *callback, bool keep_alive = true,
            uint16_t local_port = 0,
            TcpConnection::LogMode mode = TcpConnection::kVerbose);

  virtual ~TcpClient();

  bool IsConnected() const;
  sockaddr_in GetRemoteAddress() const;
  sockaddr_in GetLocalAddress() const;

  network::SocketAddress4 GetRemoteAddress2() const;
  network::SocketAddress4 GetLocalAddress2() const;

  void SetPingInterval(uint32_t seconds);
  void SetTimeout(uint32_t seconds);
  void SetBackoff(float backoff = 1.0);

  void RegisterCallback(TcpClientCallback *callback);

  bool Connect();
  bool Stop();
  void Close();
  void Flush();

  bool EnableMptcp(bool enable);

  bool SendRawData(const void *data, uint32_t length);
  bool SendPacket(const base::Packet &p);

 private:
  friend class ::agora::testing::TcpClientTester;

  void Init(event_base *base, const network::SocketAddress4 &remote,
            TcpClientCallback *callback, bool keep_alive, uint16_t local_port,
            TcpConnection::LogMode mode, float backoff = 1.0);

  void OnConnect(int events);
  void OnTimer();

  bool TurnOnMptcp(int fd);

  void ScheduleNextTry();

  // Inherited from |TcpConnectionCallback|
  virtual void OnError(TcpConnection *conn, int events);
  virtual void OnPacketReceived(TcpConnection *conn, base::Unpacker *p,
                                uint16_t server_type, uint16_t uri);

 private:
  static void ConnectCallback(
      bufferevent *bev, short events,  // NOLINT(runtime/int)
      void *context);

 private:
  TcpConnection::LogMode mode_;

  event_base *base_;
  network::SocketAddress4 addr_;
  TcpClientCallback *callback_;
  base::RepeatedTimer timer_;

  uint16_t local_port_;

  bufferevent *pending_;
  TcpConnection *conn_;
  bool connecting_;
  bool stopped_;
  bool mptcp_enabled_;

  // the interval in seconds to determine if a connection is alive.
  int32_t reconnect_timeout_;

  // in seconds
  int32_t last_try_ts_;

  int32_t next_try_ts_;

  float backoff_;
  int32_t next_try_interval_;

  int32_t last_active_ts_;
};

}  // namespace network
}  // namespace agora
