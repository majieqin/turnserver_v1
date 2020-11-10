// Copyright (c) 2014-2017 Agora.io, Inc.
// A generic tcp listening class to facilize to accept tcp connections
// from clients.
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <string>

#include "event2/event.h"
#include "event2/listener.h"

#include "media_server_library/base/cpp_feature.h"
#include "media_server_library/network/address_types.h"

namespace agora {
namespace base {

class EventLoop;

}  // namespace base

namespace network {

class GenericTcpServer : private base::NonCopyOrMovable {
 public:
  struct Visitor;

  GenericTcpServer(event_base *base, Visitor *listener);
  GenericTcpServer(base::EventLoop *loop, Visitor *listener);

  virtual ~GenericTcpServer();

  // Host-ordered
  bool ListenOn(uint16_t port, uint32_t local_address = INADDR_ANY,
                int backlog = -1);
  bool ListenOnV6(uint16_t port, const Ipv6 &ipv6 = network::Ipv6::Any(),
                  int backlog = -1);
  bool ListenOnUnix(const char *path, int backlog = -1);

  bool EnableMptcp(bool enable);

  void EnableReusePort();

 private:
  bool EnableReusePortImpl();
  bool IsListening() const;
  bool SetCommonOpt();

  evconnlistener *BindAndListen(uint16_t port, uint32_t local_address,
                                int backlog);
  evconnlistener *BindAndListenV6(uint16_t port, const Ipv6 &ipv6,
                                int backlog);
  evconnlistener *BindAndListenUnix(const char *path, int backlog);

  void AcceptConnection(evconnlistener *listener, evutil_socket_t fd,
                        sockaddr *addr, int socklen);

  static void AcceptCallback(evconnlistener *listener, evutil_socket_t fd,
                             sockaddr *addr, int socklen, void *context);

 private:
  event_base *base_;
  evconnlistener *conn_;
  Visitor *callback_;
  bool enable_reuse_port_;

  int fd_;
  std::string path_;
};

struct GenericTcpServer::Visitor {
  // NOTE: It's the user's duty to manage |fd|
  virtual void OnConnectionAccepted(
      int fd, const network::GeneralSocketAddress &client_addr) = 0;

 protected:
  virtual ~Visitor();
};

inline void GenericTcpServer::EnableReusePort() {
  enable_reuse_port_ = true;
}

typedef GenericTcpServer::Visitor GenericTcpServerCallback;

}  // namespace network
}  // namespace agora
