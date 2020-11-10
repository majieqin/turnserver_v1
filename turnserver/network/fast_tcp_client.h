// Copyright (c) 2014-2017 Agora.io, Inc.
// TODO(liuyong): Gives a detailed description
//

#pragma once  // NOLINT(build/header_guard)

#include <arpa/inet.h>

#include <cstdint>
#include <vector>
#include <memory>

#include "event2/event.h"
#include "event2/bufferevent.h"

#include "network/address_types.h"
#include "base/cpp_feature.h"
#include "base/event_loop.h"
#include "base/timer.h"
#include "base/time_util.h"

#include "network/fast_tcp_connection.h"

namespace agora {
namespace base {
class EventLoop;
class Unpacker;
}  // namespace base

namespace network {

namespace testing {
class FastTcpClientTester;
}  // namespace testing

class FastTcpClient : private FastTcpConnectionCallback,
                      private base::NonCopyOrMovable {
 public:
  enum KeepAliveMode : uint8_t {
    kNoKeepAlive = 0,
    kKeepAlive = 1,
  };

  struct Visitor;

  typedef FastTcpConnection::CacheMode CacheMode;

  FastTcpClient(event_base *base, const network::NewSockAddress4 &remote,
                Visitor *callback,
                KeepAliveMode mode = kKeepAlive, uint16_t local_port = 0,
                CacheMode cache_mode = CacheMode::kDirect);

  FastTcpClient(base::EventLoop *loop, const network::NewSockAddress4 &remote,
                Visitor *callback,
                KeepAliveMode mode = kKeepAlive, uint16_t local_port = 0,
                CacheMode cache_mode = CacheMode::kDirect);

  virtual ~FastTcpClient();

  bool IsConnected() const;
  network::NewSockAddress4 GetRemoteAddress() const;
  network::NewSockAddress4 GetLocalAddress() const;

  bool SetSocketBufferSize(int32_t size);
  void SetMaximumCacheSize(uint32_t cache_size);

  void SetCacheMode(CacheMode cache_mode, uint32_t cache_size);
  void Flush();

  void SetBackoff(float backoff);

  // Only valid if |kKeepAlive| is set
  void SetPingInterval(uint32_t seconds);

  // Only valid if |kKeepAlive| is set
  void SetTimeout(uint32_t seconds);

  Visitor* RegisterCallback(Visitor *callback);

  bool EnableMptcp(bool enable);

  bool Connect();
  bool Stop();
  void Close();
  void Renew();

  bool SendPacket(const base::Packet &p);
  bool SendBuffer(std::vector<char> buffer);
  bool SendBuffer(memory::MemSlice buffer);
  bool SendBuffer(memory::MemSliceSpan buffer);

 private:
  friend class testing::FastTcpClientTester;

  struct BufferEventDeleter {
    void operator()(::bufferevent *e) const;
  };

  struct ConnectionDeleter {
    void operator()(FastTcpConnection *c) const;
  };

  void OnConnect(int events);
  void OnTimer();

  bool TurnOnMptcp(int fd);

  void ScheduleNextTry();

  void CloseInternal(bool notify_visitor = true);

  // Inherited from |FastTcpConnectionCallback|
  virtual void OnPacketReceived(FastTcpConnection *conn, base::Unpacker *p,
                                uint16_t server_type, uint16_t uri) OVERRIDE;

  virtual void OnError(FastTcpConnection *conn, int events) OVERRIDE;

 private:
  static void ConnectCallback(
      bufferevent *bev, short events,  // NOLINT(runtime/int)
      void *context);

 private:
  event_base *base_;
  network::NewSockAddress4 addr_;
  Visitor *callback_;
  base::RepeatedTimer timer_;

  uint16_t local_port_;

  std::unique_ptr<bufferevent, BufferEventDeleter> pending_;
  std::unique_ptr<FastTcpConnection, ConnectionDeleter> conn_;

  bool connecting_;
  bool stopped_;
  bool mptcp_enabled_;

  KeepAliveMode keep_alive_mode_;
  // the interval in seconds to determine if a connection is alive.
  int32_t reconnect_timeout_;

  // in seconds
  int32_t last_try_ts_;
  int32_t next_try_ts_;

  float backoff_;
  int32_t next_try_interval_;

  int32_t last_active_ts_;

  CacheMode cache_mode_;
  uint32_t cache_size_;
  uint32_t max_cache_size_;
  int32_t socket_buffer_size_;
};

struct FastTcpClient::Visitor {
  virtual void OnConnect(FastTcpClient *conn, bool connected) = 0;
  virtual void OnPacketReceived(FastTcpClient *conn, base::Unpacker *pkr,
                                uint16_t service_type, uint16_t uri) = 0;

  virtual void OnSocketError(FastTcpClient *conn) = 0;
  virtual void TimeToPing(FastTcpClient *conn) = 0;

 protected:
  virtual ~Visitor() {}
};

typedef FastTcpClient::Visitor FastTcpClientCallback;

inline void FastTcpClient::Close() {
  CloseInternal(false);
}

inline void FastTcpClient::Renew() {
  last_active_ts_ = base::TickInSeconds();
}

inline void FastTcpClient::SetMaximumCacheSize(uint32_t cache_size) {
  max_cache_size_ = cache_size;
  if (conn_) {
    conn_->SetMaximumCacheSize(cache_size);
  }
}

}  // namespace network
}  // namespace agora
