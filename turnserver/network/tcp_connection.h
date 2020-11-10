// Copyright (c) 2014-2017 Agora.io, Inc.
// A base class helping to manage sending & receiving data from/to a tcp socket
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>

#include <event2/buffer.h>  // NOLINT(build/include_order)
#include <event2/bufferevent.h>  // NOLINT(build/include_order)
#include <event2/event.h>  // NOLINT(build/include_order)
#include <event2/listener.h>  // NOLINT(build/include_order)

#include "address_types.h"

namespace agora {
namespace base {

class EventLoop;
class Packet;
class Unpacker;

}  // namespace base

namespace network {

class TcpConnection;

struct TcpConnectionCallback {
  // This connection was closed by the remote peer.
  virtual void OnError(TcpConnection *conn,
                       int events) = 0;

  virtual void OnPacketReceived(
      TcpConnection *conn, base::Unpacker *pkr,
      uint16_t service_type, uint16_t uri) = 0;

 protected:
  virtual ~TcpConnectionCallback() {}
};

class TcpConnection {
 public:
  enum LogMode {
    kVerbose = 0,
    kSilent = 1,
  };

  static TcpConnection* Create(
      base::EventLoop *loop, evutil_socket_t fd, LogMode mode = kVerbose);

  static TcpConnection* Create(
      base::EventLoop *loop, bufferevent *bev, LogMode mode = kVerbose);

  static TcpConnection* Create(
      event_base *base, evutil_socket_t fd, LogMode mode = kVerbose);

  static TcpConnection* Create(
      event_base *base, bufferevent *bev, LogMode mode = kVerbose);

  sockaddr_in GetRemoteAddress() const;
  sockaddr_in GetLocalAddress() const;

  network::SocketAddress4 GetRemoteAddress2() const;
  network::SocketAddress4 GetLocalAddress2() const;

  // Returns the underlying fd and detachs from it.
  int Detach();

  bool EnableTcpNoDelay();

  bool IsClosed() const;
  event_base* GetEventBase() const;

  void RegisterCallback(TcpConnectionCallback *callback);

  void Flush();

  // close the socket and destroy this connection object
  void CloseAndDestroy();

  void SendRawData(const void *data, uint32_t length);
  void SendPacket(const base::Packet &p);

 private:
  TcpConnection(event_base *base, evutil_socket_t fd, LogMode mode);
  TcpConnection(base::EventLoop *loop, evutil_socket_t fd, LogMode mode);
  TcpConnection(event_base *base, bufferevent *bev, LogMode mode);
  TcpConnection(base::EventLoop *loop, bufferevent *bev, LogMode mode);

  ~TcpConnection();

  void Close();
  void Destroy();

  void SetSocketFd(evutil_socket_t fd);
  void FillSocketAddress(evutil_socket_t fd);
  void OnDataReady();
  void OnEvent(int events);

 private:
  static void ReadCallback(bufferevent *bev, void *context);
  static void WriteCallback(bufferevent *bev, void *context);
  static void EventCallback(bufferevent *bev,
                            short events,  // NOLINT(runtime/int)
                            void *context);

 private:
  LogMode mode_;
  bool is_in_processing_;

  event_base *base_;
  bufferevent *bev_;
  TcpConnectionCallback *callback_;

  network::SocketAddress4 remote_;
  network::SocketAddress4 local_;
};

inline bool TcpConnection::IsClosed() const {
  return bev_ == nullptr;
}

inline event_base* TcpConnection::GetEventBase() const {
  return base_;
}

}  // namespace network
}  // namespace agora
