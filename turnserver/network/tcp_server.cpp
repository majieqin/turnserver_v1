// Copyright (c) 2014-2017 Agora.io, Inc.
// Implementation of |TcpServer|
//

#include "tcp_server.h"

#include <arpa/inet.h>
#include <event2/util.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <cassert>

#include "address_types.h"
#include "common_types.h"
#include "../base/event_loop.h"
#include "../base/packer.h"
#include "../base/packet.h"

#include "../logging/log.h"
#include "../logging/safe_log.h"

#include "network_feature.h"
#include "tcp_connection.h"

namespace agora {
namespace network {

TcpServer::TcpServer(base::EventLoop *loop, TcpServerCallback *callback,
                     TcpConnection::LogMode mode)
  : TcpServer(loop->GetEventBase(), callback, mode) {
}

TcpServer::TcpServer(event_base *base, TcpServerCallback *callback,
                     TcpConnection::LogMode mode) {
  mode_ = mode;

  base_ = base;
  conn_ = nullptr;

  callback_ = callback;

  fd_ = socket(AF_INET, SOCK_STREAM, 0);

  if (fd_ == -1) {
    SAFE_LOG2(Fatal) << "Cannot create socket for tcp server!"
                     << strerror(errno);
    exit(1);
  } else {
    if (evutil_make_socket_nonblocking(fd_) == -1) {
      SAFE_LOG2(Fatal) << "Cannot set FD option for tcp server!"
                       << strerror(errno);
      exit(1);
    }
  }
}

TcpServer::~TcpServer() {
  if (conn_) {
    evconnlistener_free(conn_);
  }
  evutil_closesocket(fd_);
}

bool TcpServer::ListenOn(uint16_t port, uint32_t local_address,
                         int backlog) {
  if (conn_) {
    evutil_socket_t fd = evconnlistener_get_fd(conn_);
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    sockaddr *s_addr = reinterpret_cast<sockaddr *>(&addr);
    LOG_IF2(Error, getsockname(fd, s_addr, &len), "Failed to get bound address "
            "of fd %d", fd);

    SAFE_LOG2(Error) << "Already bind to "
                     << SocketAddress4(addr).ToDebugString();
    return false;
  }

  conn_ = BindAndListen(port, local_address, backlog);
  return conn_ != nullptr;
}

void TcpServer::RegisterCallback(TcpServerCallback *callback) {
  callback_ = callback;
}

evconnlistener *TcpServer::BindAndListen(uint16_t port,
                                         uint32_t local_address,
                                         int backlog) {
  sockaddr_in sin = {0, 0, {0}, {0}};
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = htonl(local_address);

  int on = 1;
  const void *p = reinterpret_cast<const void *>(&on);
  if (setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, p, sizeof(on)) < 0) {
    SAFE_LOG2(Error) << "Cannot set FD option for tcp server!"
                     << strerror(errno);
    return nullptr;
  }

  if (evutil_make_listen_socket_reuseable(fd_) < 0) {
    SAFE_LOG2(Error) << "Error from evutil_make_listen_socket_reuseable!";
    return nullptr;
  }

  if (bind(fd_, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    SAFE_LOG2(Error) << "Cannot bind!" << strerror(errno);
    return nullptr;
  }

  return evconnlistener_new(base_, AcceptCallback, this,
                            LEV_OPT_CLOSE_ON_FREE, backlog, fd_);
}

void TcpServer::AcceptCallback(evconnlistener *listener, evutil_socket_t fd,
                               sockaddr *addr, int addr_len, void *context) {
  TcpServer *p = reinterpret_cast<TcpServer *>(context);
  p->AcceptConnection(listener, fd, addr, addr_len);
}

void TcpServer::AcceptConnection(evconnlistener *listener, evutil_socket_t fd,
                                 sockaddr *addr, int addr_len) {
  (void)listener;
  (void)addr_len;

  assert(listener == conn_);
  assert(addr_len == sizeof(sockaddr_in));

  if (!callback_) {
    SAFE_LOG2(Error) << "Server accepting a connection "
                     << "but has no callback registered!";
    return;
  }

  const sockaddr_in &in_addr = reinterpret_cast<const sockaddr_in &>(*addr);
  LOG_IF2(Info, mode_ == TcpConnection::kVerbose,
          "accept a connection %d from %s",
          fd, SocketAddress4(in_addr).ToDebugString().c_str());

  TcpConnection *c = TcpConnection::Create(base_, fd, mode_);
  callback_->OnConnectionAccepted(c);
}

bool TcpServer::EnableMptcp(bool enable) {
  if (conn_) {
    return false;  // NOTE(Qi Ji): Must be set before binding!
  }
  int value = enable ? 1 : 0;
  const void *p = reinterpret_cast<const void *>(&value);
  if (setsockopt(fd_, SOL_TCP, MPTCP_ENABLED, p, sizeof(value)) == -1) {
    SAFE_LOG2(Error) << "Cannot enable Multipath TCP on "
                     << fd_ << ": " << strerror(errno);
    return false;
  }
  return true;
}

uint16_t TcpServer::GetListenPort() {
  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);
  if (getsockname(fd_, (struct sockaddr *)&sin, &len) == -1) {
    int error = errno;
    SAFE_LOG2(Error) << "Get server listen port failed "
                     << fd_ << strerror(error);
    return 0;
  } else {
    return ntohs(sin.sin_port);
  }
}

bool TcpServer::EnableReusePort() {
  if (conn_) {
    return false;  // NOTE(Qi Ji): Must be set before binding!
  }
  int value = 1;
  const void *p = reinterpret_cast<const void *>(&value);
  if (setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, p, sizeof(value)) == -1) {
    SAFE_LOG2(Error) << "Cannot enable Reuse Port on "
                     << fd_ << ": " << strerror(errno);
    return false;
  }
  return true;
}

}  // namespace network
}  // namespace agora
