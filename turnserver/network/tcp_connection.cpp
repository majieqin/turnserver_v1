// Copyright (c) 2014-2017 Agora.io, Inc.
// Implementation of |TcpConnection|
//

#include "tcp_connection.h"

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <cassert>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <vector>

#include "address_types.h"
#include "common_types.h"
#include "event_loop.h"
#include "packer.h"
#include "packet.h"
#include "unpacker.h"

#include "log.h"
#include "safe_log.h"

#include "network_feature.h"

#define try          if(true)
#define catch(...)   if(false)

namespace agora {
namespace network {

TcpConnection::TcpConnection(
    event_base *base, evutil_socket_t fd, LogMode mode) {
  mode_ = mode;

  SAFE_LOG_IF2(mode_ == kVerbose, Info) << "Tcp connection " << this
      << ", fd: " << fd;

  is_in_processing_ = false;

  base_ = base;
  callback_ = NULL;
  bev_ = NULL;

  if (fd != -1) {
    SetSocketFd(fd);
  }
}

TcpConnection::TcpConnection(
    base::EventLoop *loop, evutil_socket_t fd, LogMode mode)
  : TcpConnection(loop->GetEventBase(), fd, mode) {}

TcpConnection::TcpConnection(
    event_base *base, bufferevent *bev, LogMode mode) {
  mode_ = mode;

  is_in_processing_ = false;

  base_ = base;
  callback_ = NULL;

  bev_ = bev;
  bufferevent_setcb(bev_, ReadCallback, WriteCallback, EventCallback, this);
  bufferevent_enable(bev_, EV_READ | EV_WRITE);

  assert(bev_ != NULL);

  evutil_socket_t s = bufferevent_getfd(bev_);
  FillSocketAddress(s);

  SAFE_LOG_IF2(mode_ == kVerbose, Info) << "Tcp connection " << this
      << ", fd: " << s;
}

TcpConnection::TcpConnection(
    base::EventLoop *loop, bufferevent *bev, LogMode mode)
  : TcpConnection(loop->GetEventBase(), bev, mode) {}

TcpConnection::~TcpConnection() {
  SAFE_LOG_IF2(mode_ == kVerbose, Info) << "destroy tcp connection " << this;
  Close();
}

TcpConnection* TcpConnection::Create(
    base::EventLoop *loop, evutil_socket_t fd, LogMode mode) {
  return new TcpConnection(loop, fd, mode);
}

TcpConnection* TcpConnection::Create(
    base::EventLoop *loop, bufferevent *bev, LogMode mode) {
  return new TcpConnection(loop, bev, mode);
}

TcpConnection* TcpConnection::Create(
    event_base *base, evutil_socket_t fd, LogMode mode) {
  return new TcpConnection(base, fd, mode);
}

TcpConnection* TcpConnection::Create(
    event_base *base, bufferevent *bev, LogMode mode) {
  return new TcpConnection(base, bev, mode);
}

sockaddr_in TcpConnection::GetRemoteAddress() const {
  return static_cast<sockaddr_in>(remote_);
}

sockaddr_in TcpConnection::GetLocalAddress() const {
  return static_cast<sockaddr_in>(local_);
}

network::SocketAddress4 TcpConnection::GetRemoteAddress2() const {
  return remote_;
}

network::SocketAddress4 TcpConnection::GetLocalAddress2() const {
  return local_;
}

void TcpConnection::FillSocketAddress(evutil_socket_t s) {
  sockaddr_in remote;
  socklen_t len = sizeof(remote);

  if (!getpeername(s, reinterpret_cast<sockaddr *>(&remote), &len)) {
    // FIXME(liuyong): what if IPv6
    assert(len ==  sizeof(remote));
    remote_ = remote;
  } else {
    LOG2(Warn, "Failed to get the remote addr: %d", errno);
  }

  sockaddr_in local;
  len = sizeof(local);

  if (!getsockname(s, reinterpret_cast<sockaddr *>(&local), &len)) {
    assert(len == sizeof(local));
    local_ = local;
  } else {
    LOG2(Warn, "Failed to get the local addr: %d", errno);
  }
}

void TcpConnection::RegisterCallback(TcpConnectionCallback *callback) {
  callback_ = callback;
}

void TcpConnection::SetSocketFd(evutil_socket_t fd) {
  assert(bev_ == nullptr);

  bev_ = bufferevent_socket_new(base_, fd, BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb(bev_, ReadCallback, WriteCallback, EventCallback, this);
  bufferevent_enable(bev_, EV_READ | EV_WRITE);

  FillSocketAddress(fd);
}

void TcpConnection::Close() {
  if (bev_) {
    bufferevent_free(bev_);
    bev_ = NULL;
  }
}

void TcpConnection::Destroy() {
  delete this;
}

void TcpConnection::CloseAndDestroy() {
  Close();

  // delay this destruction
  if (!is_in_processing_)
    Destroy();
}

void TcpConnection::SendPacket(const base::Packet &p) {
  if (IsClosed())
    return;

  // FIXME(liuyong)
  base::Packer pk;
  p.Pack(&pk);

  bufferevent_write(bev_, pk.Buffer(), pk.Length());
}

void TcpConnection::SendRawData(const void *buff, uint32_t length) {
  if (IsClosed())
    return;

  bufferevent_write(bev_, buff, length);
}

void TcpConnection::OnDataReady() {
  // FIXME(liuyong)
  evbuffer *input = bufferevent_get_input(bev_);
  uint32_t packet_length = 0;  // packet length 2-3 bytes

  is_in_processing_ = true;

  static const uint32_t kMinPacketLen = 6;

  while (!IsClosed()) {
    size_t length = evbuffer_get_length(input);
    if (length < kMinPacketLen)
      break;

    uint16_t lower = 0;
    evbuffer_copyout(input, &lower, sizeof(lower));
    if (lower >= 0x8000) {
      uint32_t value;
      evbuffer_copyout(input, &value, sizeof(value));
      value &= 0xFFFFFF;

      packet_length = (lower & 0x7FFF) + ((value >> 16) << 15);
    } else {
      packet_length = lower;
    }

    if (packet_length < kMinPacketLen) {
      if (callback_)
        callback_->OnError(this, BEV_EVENT_ERROR | BEV_EVENT_READING);
      break;
    }

    if (length < packet_length)
      break;

    std::vector<char> buffer(packet_length);

    size_t read = bufferevent_read(bev_, &buffer[0], packet_length);
    if (read == (size_t)(-1)) {
      LOG2(Error, "read data from socket %d(%s<->%s) error",
           GetRemoteAddress2().ToDebugString().c_str(),
           GetLocalAddress2().ToDebugString().c_str());
      break;
    }

    base::Unpacker p(&buffer[0], packet_length);

    uint16_t server_type = p.PopUint16();
    uint16_t uri = p.PopUint16();
    p.Rewind();

    try {
      if (callback_)
        callback_->OnPacketReceived(this, &p, server_type, uri);
    } catch (const std::exception &e) {
      LOG_IF2(Error, mode_ == kVerbose, "tcp: %p, error on packet %u %u",
              this, server_type, uri);

      // FIXME(liuyong): An exception is thrown
      callback_->OnError(this, 0x80);
      break;
    }
  }

  is_in_processing_ = false;

  if (IsClosed())
    Destroy();
}

void TcpConnection::OnEvent(int events) {
  int s = bufferevent_getfd(bev_);
  if (events & (BEV_EVENT_ERROR | BEV_EVENT_EOF)) {
    LOG_IF2(Notice, mode_ == kVerbose, "socket %x %x %u %s error 0x%x", this,
            bev_, s, GetRemoteAddress2().ToDebugString().c_str(),
            events);

    if (callback_)
      callback_->OnError(this, events);
  }
}

void TcpConnection::ReadCallback(bufferevent *bev, void *context) {
  (void)bev;

  TcpConnection *c = reinterpret_cast<TcpConnection *>(context);
  c->OnDataReady();
}

void TcpConnection::WriteCallback(bufferevent *bev, void *context) {
  (void)bev;
  (void)context;
  // do nothing
}

void TcpConnection::EventCallback(bufferevent *bev,
                                  short events,  // NOLINT(runtime/int)
                                  void *context) {
  (void)bev;

  TcpConnection *c = reinterpret_cast<TcpConnection *>(context);
  c->OnEvent(events);
}

void TcpConnection::Flush() {
  if (bev_) {
    bufferevent_flush(bev_, EV_WRITE, BEV_FINISHED);
  }
}

int TcpConnection::Detach() {
  if (!bev_) {
    return -1;
  }

  int fd = bufferevent_getfd(bev_);
  bufferevent_setfd(bev_, -1);

  return fd;
}

bool TcpConnection::EnableTcpNoDelay() {
  if (!bev_)
    return false;

  int fd = bufferevent_getfd(bev_);
  if (fd < 0)
    return false;

  // FIXME(liuyong)
  // int value = TCP_NODELAY;
  // if (setsockopt(fd, IPPROTO_TCP, value, sizeof(value)) == -1) {
  //   SAFE_LOG2(Error) << "Failed to set TCP_NODELAY:" << strerror(errno);
  //   return false;
  // }

  return true;
}

}  // namespace network
}  // namespace agora
