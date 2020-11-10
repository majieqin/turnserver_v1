// Copyright (c) 2014-2017 Agora.io, Inc.
// A keep-alive long tcp client implementation
//

#include "tcp_client.h"

#include <unistd.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include <algorithm>
#include <cassert>

#include "event2/util.h"

#include "packet.h"
#include "time_util.h"

#include "address_types.h"

#include "log.h"
#include "safe_log.h"

#include "network_feature.h"

namespace agora {
namespace network {

namespace {
enum {
  kMicroPerSec = 1000 * 1000llu,
  kKeepAliveIntervalMs = 3000,
  kReconnectTimeout = 10,
  kShortConnectionInterval = 60,
  kMaxTimeout = 300,
};

}  // namespace

// Event flow
// A |Connect| event is followed by a |OnConnect(false)|, |OnSocketError| or
// |Close| in the timer to terminate the current connection.
TcpClient::TcpClient(base::EventLoop *loop, uint32_t ip, uint16_t port,
                     TcpClientCallback *callback, bool keep_alive,
                     uint16_t local_port, TcpConnection::LogMode mode)
    : TcpClient(loop->GetEventBase(), ip, port, callback,
                keep_alive, local_port, mode) {
}

TcpClient::TcpClient(base::EventLoop *loop, const sockaddr_in &remote,
                     TcpClientCallback *callback, bool keep_alive,
                     uint16_t local_port, TcpConnection::LogMode mode)
    : TcpClient(loop->GetEventBase(), remote, callback,
                keep_alive, local_port, mode) {
}

TcpClient::TcpClient(base::EventLoop *loop, const SocketAddress4 &remote,
                     TcpClientCallback *callback, bool keep_alive,
                     uint16_t local_port, TcpConnection::LogMode mode)
    : TcpClient(loop->GetEventBase(), remote, callback,
                keep_alive, local_port, mode) {
}

TcpClient::TcpClient(base::EventLoop *loop, const GeneralSocketAddress &remote,
                     TcpClientCallback *callback, bool keep_alive,
                     uint16_t local_port, TcpConnection::LogMode mode)
    : TcpClient(loop->GetEventBase(), remote, callback,
                keep_alive, local_port, mode) {
}

TcpClient::TcpClient(event_base *base, uint32_t ip, uint16_t port,
                     TcpClientCallback *callback, bool keep_alive,
                     uint16_t local_port, TcpConnection::LogMode mode)
    : TcpClient(base, network::SocketAddress4(htonl(ip), htons(port)),
                callback, keep_alive, local_port, mode) {
}

TcpClient::TcpClient(event_base *base, const sockaddr_in &remote,
                     TcpClientCallback *callback, bool keep_alive,
                     uint16_t local_port, TcpConnection::LogMode mode)
    : TcpClient(base, network::SocketAddress4(remote), callback, keep_alive,
                local_port, mode) {
}

TcpClient::TcpClient(event_base *base, const SocketAddress4 &remote,
                     TcpClientCallback *callback, bool keep_alive,
                     uint16_t local_port, TcpConnection::LogMode mode) {
  Init(base, remote, callback, keep_alive, local_port, mode);
}

TcpClient::TcpClient(event_base *base, const GeneralSocketAddress &remote,
                     TcpClientCallback *callback, bool keep_alive,
                     uint16_t local_port, TcpConnection::LogMode mode) {
  if (remote.GetSocketAddressType() == IpVersion::kIp4) {
    Init(base, SocketAddress4(remote), callback, keep_alive, local_port, mode);
  } else {
    SAFE_LOG2(Fatal) << "Incorrect Ip Version When Initializing Tcp Client!";
    abort();
  }
}

TcpClient::~TcpClient() {
  Close();
}

bool TcpClient::IsConnected() const {
  return !connecting_ && conn_;
}

sockaddr_in TcpClient::GetRemoteAddress() const {
  return static_cast<sockaddr_in>(addr_);
}

network::SocketAddress4 TcpClient::GetRemoteAddress2() const {
  return addr_;
}

sockaddr_in TcpClient::GetLocalAddress() const {
  assert(conn_ != nullptr);

  sockaddr_in addr;
  addr.sin_family = AF_INET;

  if (conn_) {
    return conn_->GetLocalAddress();
  }

  return addr;
}

network::SocketAddress4 TcpClient::GetLocalAddress2() const {
  assert(conn_ != nullptr);

  network::SocketAddress4 addr;
  if (conn_) {
    return conn_->GetLocalAddress2();
  }

  return addr;
}

void TcpClient::Init(
    event_base *base, const network::SocketAddress4 &remote,
    TcpClientCallback *callback, bool keep_alive, uint16_t local_port,
    TcpConnection::LogMode mode, float backoff) {
  (void)keep_alive;

  mode_ = mode;

  base_ = base;
  addr_ = remote;
  callback_ = callback;
  timer_ = base::RepeatedTimer(base_, kKeepAliveIntervalMs,
                               &TcpClient::OnTimer, this);

  local_port_ = local_port;
  pending_ = nullptr;
  conn_ = nullptr;
  connecting_ = false;
  stopped_ = false;
  mptcp_enabled_ = false;

  reconnect_timeout_ = kReconnectTimeout;

  last_try_ts_ = 0;
  next_try_ts_ = 0;

  backoff_ = backoff;
  next_try_interval_ = reconnect_timeout_;

  last_active_ts_ = base::TickInSeconds();
}

void TcpClient::SetPingInterval(uint32_t seconds) {
  timer_ = base::RepeatedTimer(base_, seconds * 1000, &TcpClient::OnTimer,
                               this);
}

void TcpClient::SetTimeout(uint32_t seconds) {
  reconnect_timeout_ = std::max<uint32_t>(5u, seconds);
}

void TcpClient::RegisterCallback(TcpClientCallback *callback) {
  callback_ = callback;
}

static inline bool SetNonBlocking(int fd) {
  if (evutil_make_socket_nonblocking(fd) != 0) {
    return false;
  }

  return true;
}

bool TcpClient::Connect() {
  if (connecting_)
    return true;

  last_try_ts_ = base::TickInSeconds();
  last_active_ts_ = last_try_ts_;

  if (conn_) {
    conn_->CloseAndDestroy();
    conn_ = nullptr;
  }

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    SAFE_LOG2(Error) << "Cannot create socket for tcp client"
                     << strerror(errno);
    return false;
  }

  if (!SetNonBlocking(fd)) {
    SAFE_LOG2(Error) << "Failed to set nonblocking mode for " << fd;
    close(fd);
    return false;
  }

  if (mptcp_enabled_ && !TurnOnMptcp(fd)) {
    SAFE_LOG2(Debug) << "Cannot turn mptcp on for fd " << fd;
  }

  connecting_ = true;
  pending_ = ::bufferevent_socket_new(base_, fd, BEV_OPT_CLOSE_ON_FREE);
  ::bufferevent_setcb(pending_, nullptr, nullptr, ConnectCallback, this);

  if (local_port_ != 0) {
    sockaddr_in local = {0, 0, {0}, {0}};
    local.sin_family = AF_INET;
    local.sin_port = htons(local_port_);

    const sockaddr *addr = reinterpret_cast<const sockaddr *>(&local);
    if (::bind(fd, addr, sizeof(local)) != 0) {
      SAFE_LOG2(Error) << "Failed to bind to local port: " << local_port_;
      ::bufferevent_free(pending_);
      pending_ = nullptr;
      connecting_ = false;

      return false;
    }
  }

  sockaddr_in addr = static_cast<sockaddr_in>(addr_);
  sockaddr *p = reinterpret_cast<sockaddr *>(&addr);
  if (::bufferevent_socket_connect(pending_, p, sizeof(addr)) < 0) {
    // NOTE(liuyong): If there is no route to the destination, |connect|
    // will return immediately, and |connect_callback| will get invoked and
    // |bufferevent_free| will be called, so don't attempt to free |bev| here.
    // ::bufferevent_free(bev);
    connecting_ = false;
    return false;
  }

  return true;
}

void TcpClient::ConnectCallback(
    bufferevent *bev, short events,  // NOLINT(runtime/int)
    void *context) {
  (void)bev;

  TcpClient *p = reinterpret_cast<TcpClient *>(context);
  assert(bev == p->pending_);

  p->OnConnect(events);
}

void TcpClient::OnConnect(int events) {
  assert(pending_ != nullptr);
  assert(conn_ == nullptr);
  assert(connecting_);

  connecting_ = false;

  if (events & BEV_EVENT_CONNECTED) {
    int s = bufferevent_getfd(pending_);
    LOG_IF2(Info, mode_ == TcpConnection::kVerbose, "Connected to %d, %s", s,
            addr_.ToDebugString().c_str());

    conn_ = TcpConnection::Create(base_, pending_, mode_);
    conn_->RegisterCallback(this);
    pending_ = nullptr;

    callback_->OnConnect(this, true);
  } else if (events & BEV_EVENT_ERROR) {
    ScheduleNextTry();

    bufferevent_free(pending_);
    pending_ = nullptr;

    callback_->OnConnect(this, false);
  } else {
    LOG2(Fatal, "Should not fall through here");
    pending_ = nullptr;
  }
}

bool TcpClient::SendRawData(const void *data, uint32_t length) {
  if (!IsConnected()) {
    return false;
  }

  conn_->SendRawData(data, length);
  return true;
}

bool TcpClient::SendPacket(const base::Packet &p) {
  if (!IsConnected()) {
    LOG2(Debug, "cannot send message %u %u to %s, not connected",
         p.server_type(), p.uri(), addr_.ToDebugString().c_str());
    return false;
  }

  conn_->SendPacket(p);
  return true;
}

void TcpClient::ScheduleNextTry() {
  auto cur_tick = base::TickInSeconds();

  if (last_try_ts_ + kShortConnectionInterval >= cur_tick) {
    next_try_ts_ = cur_tick + next_try_interval_;
    next_try_interval_ =
        std::min<int32_t>(kMaxTimeout, backoff_ * next_try_interval_);
  } else {
    next_try_interval_ = reconnect_timeout_;
    next_try_ts_ = 0;
  }
}

void TcpClient::OnError(TcpConnection *conn, int events) {
  assert(conn == conn_);
  (void)conn;
  (void)events;

  ScheduleNextTry();
  Close();
  callback_->OnSocketError(this);
}

void TcpClient::OnPacketReceived(TcpConnection *conn, base::Unpacker *p,
                                 uint16_t server_type, uint16_t uri) {
  assert(conn == conn_);
  (void)conn;

  last_active_ts_ = base::TickInSeconds();
  callback_->OnPacketReceived(this, p, server_type, uri);
}

void TcpClient::OnTimer() {
  // check if the connection is alive
  auto cur_tick = base::TickInSeconds();
  if (IsConnected() &&
      (last_active_ts_ + reconnect_timeout_ < cur_tick)) {
    LOG_IF2(Info, mode_ == TcpConnection::kVerbose, "Timeout for %s, close",
            addr_.ToDebugString().c_str());

    ScheduleNextTry();
    Close();
    return;
  }

  if (IsConnected()) {
    callback_->TimeToPing(this);
  } else if (!connecting_ && next_try_ts_ <= cur_tick) {
    Connect();
  }
}

void TcpClient::Close() {
  // A connection is pending
  if (pending_) {
    assert(conn_ == nullptr);
    connecting_ = false;
    bufferevent_free(pending_);
    pending_ = nullptr;

    return;
  }

  assert(connecting_ == false);

  if (conn_) {
    conn_->CloseAndDestroy();
    conn_ = nullptr;
  }
}

void TcpClient::Flush() {
  if (conn_) {
    conn_->Flush();
  }
}

bool TcpClient::EnableMptcp(bool enable) {
  mptcp_enabled_ = enable;
  return true;
}

bool TcpClient::TurnOnMptcp(int fd) {
  if (conn_ || connecting_ || pending_) {
    return false;  // NOTE(Qi Ji): Must be set before connect!
  }
  int value = 1;
  const void *p = reinterpret_cast<const void *>(&value);
  if (setsockopt(fd, SOL_TCP, MPTCP_ENABLED, p, sizeof(value)) == -1) {
    SAFE_LOG2(Error) << "Cannot enable Multipath TCP on "
                     << fd << ": " << strerror(errno);
  }
  return true;
}

void TcpClient::SetBackoff(float backoff) {
  backoff_ = backoff;
}

}  // namespace network
}  // namespace agora
