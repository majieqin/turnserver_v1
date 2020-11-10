// Copyright (c) 2014-2017 Agora.io, Inc.
// A tcp server helping to accept tcp connections from clients
//

#pragma once  // NOLINT(build/header_guard)

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <cstdint>

#include "tcp_connection.h"

namespace agora {
namespace base {

class EventLoop;

}  // namespace base

namespace network {

class TcpConnection;

struct TcpServerCallback {
  // NOTE: It's the user's duty to manage |conn|
  virtual void OnConnectionAccepted(TcpConnection *conn) = 0;

 protected:
  virtual ~TcpServerCallback() {}
};

class TcpServer {
 public:
  TcpServer(event_base *base, TcpServerCallback *callback,
            TcpConnection::LogMode mode = TcpConnection::kVerbose);

  TcpServer(base::EventLoop *loop, TcpServerCallback *callback,
            TcpConnection::LogMode mode = TcpConnection::kVerbose);

  ~TcpServer();

  // host-order
  bool ListenOn(uint16_t port, uint32_t local_address = INADDR_ANY,
                int backlog = -1);

  void RegisterCallback(TcpServerCallback *callback);

  bool EnableMptcp(bool enable);

  bool EnableReusePort();

  uint16_t GetListenPort();

 private:
  TcpServer(const TcpServer &) = delete;
  TcpServer& operator=(const TcpServer &) = delete;
  TcpServer(TcpServer &&) = delete;
  TcpServer& operator=(TcpServer &&) = delete;

 private:
  evconnlistener *BindAndListen(uint16_t port, uint32_t local_address,
                                int backlog);

  void AcceptConnection(evconnlistener *listener, evutil_socket_t fd,
                        sockaddr *addr, int socklen);

  static void AcceptCallback(evconnlistener *listener, evutil_socket_t fd,
                             sockaddr *addr, int socklen, void *context);

 private:
  TcpConnection::LogMode mode_;

  int fd_;
  event_base *base_;
  evconnlistener *conn_;
  TcpServerCallback *callback_;
};

}  // namespace network
}  // namespace agora
