// Copyright (c) 2014-2017 Agora.io, Inc.
// A base class for allocating a port through which all udp packets send/receive
// FIXME(liuyong): Such an implementation is totally ugly, really really bad.

#pragma once  // NOLINT(build/header_guard)

#include <netinet/in.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "media_server_library/base/cpp_feature.h"
#include "media_server_library/base/packet.h"

#include "media_server_library/network/address_types.h"
#include "media_server_library/network/udp_socket_impl.h"

struct event_base;

namespace agora {
namespace base {

class Unpacker;
class EventLoop;

}  // namespace base

namespace network {

class UdpSocketImpl;
class UdpSocket;

struct UdpEventCallback {
  virtual void OnPacketReceived(
      UdpSocket *skt, const GeneralSocketAddress &local,
      const GeneralSocketAddress &remote, base::Unpacker *,
      uint16_t service_type, uint16_t uri) = 0;

  virtual void OnPacketSent(
      UdpSocket *impl, const GeneralSocketAddress &from,
      const GeneralSocketAddress &to, uint16_t service_type,
      uint16_t uri, const void *data, size_t len);

  virtual void OnWriteReady(UdpSocket *impl);

 protected:
  virtual ~UdpEventCallback() {}
};

enum class CacheMode {
  kCache = 0,
  kNoCache = 1,
};

class UdpSocket : private base::NonCopyOrMovable, private UdpCallbackInternal {
 public:
  explicit UdpSocket(event_base *loop, UdpEventCallback *c = nullptr,
                     IpVersion ip_version = IpVersion::kIp4,
                     CacheMode cache = CacheMode::kCache);

  explicit UdpSocket(base::EventLoop *loop, UdpEventCallback *c = nullptr,
                     IpVersion ip_version = IpVersion::kIp4,
                     CacheMode cache = CacheMode::kCache);
  ~UdpSocket();

  uint16_t GetLocalPort() const;
  int GetUnderlyingFd() const;

  const GeneralSocketAddress& GetLocalAddress() const;

  // host-ordered (ip, port)
  bool Bind4(uint16_t port, uint32_t ip = INADDR_ANY);

  // NOTE(liuyong): scope_id is ignored now
  // host-ordered (in6_addr, port)
  bool Bind6(uint16_t port, const Ipv6 &ipv6 = Ipv6::Any());

  int Detach();

  UdpEventCallback* RegisterEventCallback(UdpEventCallback *c);

  // Only valid if |cache_mode_| is set to kCache
  bool SetAutoFlushInterval(int interval_ms = 10);

  bool SendPacket(const GeneralSocketAddress &remote, const base::Packet &pkt,
                  uint16_t *packet_length = nullptr);

  bool SendRawData(
      const GeneralSocketAddress &remote, std::vector<char> buffer);

  bool SendRawData(
      const GeneralSocketAddress &remote, const void *buf, size_t length);

  int Flush(FlushMode mode = FlushMode::kDropFailed);

  // A page is 4096 bytes.
  void AdjustUdpBufferSize(uint32_t pages);

  bool EnableReusePort();

 private:
  virtual void OnPacketReceived(
      UdpSocketImpl *impl, const sockaddr_storage &remote, const void *data,
      size_t len) OVERRIDE;

  virtual void OnPacketSent(
      UdpSocketImpl *impl, const sockaddr_storage &remote, const void *data,
      size_t len) OVERRIDE;

  virtual void OnWriteReady(UdpSocketImpl *impl) OVERRIDE;

 private:
  static const size_t kDefaultUdpBufferPages = 4096;
  static const size_t kPendingPacketCount = 8196;

 private:
  event_base *loop_;
  CacheMode cache_mode_;
  UdpEventCallback *callback_;

  int socket_;
  GeneralSocketAddress local_address_;

  struct UdpSocketImplDeleter {
    void operator()(UdpSocketImpl *impl) const;
  };

  std::unique_ptr<UdpSocketImpl, UdpSocketImplDeleter> impl_;
};

inline uint16_t UdpSocket::GetLocalPort() const {
  return local_address_.GetPort();
}

inline int UdpSocket::GetUnderlyingFd() const {
  return socket_;
}

inline const GeneralSocketAddress& UdpSocket::GetLocalAddress() const {
  return local_address_;
}

}  // namespace network
}  // namespace agora
