// Copyright (c) 2014-2017 Agora.io, Inc.
// For managing multiple Udp sockets bound to multi NICs.
// FIXME(liuyong): Such an implementation is totally ugly, really really bad.

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

#include "media_server_library/base/cpp_feature.h"
#include "media_server_library/base/packet.h"

#include "media_server_library/network/address_types.h"
#include "media_server_library/network/udp_socket.h"

struct event_base;

namespace agora {
namespace network {

class UdpSocketWithCounter;

class UdpSocket4Helper : private base::NonCopyOrMovable {
 public:
  explicit UdpSocket4Helper(
      event_base *loop, UdpEventCallback *callback,
      CacheMode cache = CacheMode::kCache);

  ~UdpSocket4Helper();

  uint16_t GetLocalPort() const;

  void EnableReusePort();

  // |*port| is both in and out
  bool TryBind(uint16_t *port, int max_tries, bool all_ip = false);

  // |local_ip| should be host-ordered
  // Return value: -1 means error
  bool SendPacket(uint32_t local_ip, const SocketAddress4 &remote,
                  const base::Packet &pkt);

  bool SendBuffer(uint32_t local_ip, const SocketAddress4 &remote,
                  const void *buffer, size_t length);

  // FIXME(liuyong)
  void SetFlushInterval(int32_t interval_ms);

  UdpSocket* GetSocketToFirstNonLocalIp() const;

  // |local_ip| should be host-ordered
  UdpSocket* GetSocketByIp(uint32_t local_ip) const;

  std::string GetStatisticsInJsonFormat(int64_t ts) const;
  std::string GetStatisticsInPlainText(int64_t ts) const;

 private:
  typedef std::unique_ptr<UdpSocketWithCounter> UdpSocketDataPtr;

 private:
  event_base *loop_;
  uint16_t port_;
  UdpEventCallback *callback_;
  CacheMode cache_mode_;
  bool enable_reuse_port_{false};

  std::unordered_map<uint32_t, UdpSocketDataPtr> sockets_;
};

inline uint16_t UdpSocket4Helper::GetLocalPort() const {
  return port_;
}

}  // namespace network
}  // namespace agora
