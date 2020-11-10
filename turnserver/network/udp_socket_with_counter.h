// Copyright (c) 2014-2017 Agora.io, Inc.
// Implementation of a UdpSocket with counting
// FIXME(liuyong): Such an implementation is totally ugly, really really bad.

#pragma once  // NOLINT(build/header_guard)

#include <memory>
#include <string>

#include "media_server_library/base/cpp_feature.h"

#include "media_server_library/network/packet_counter.h"
#include "media_server_library/network/udp_socket.h"

namespace agora {
namespace network {

typedef std::unique_ptr<UdpSocket> UdpSocketPtr;

class UdpSocketWithCounter : private base::NonCopyable,
                             private UdpEventCallback {
 public:
  explicit UdpSocketWithCounter(
      UdpEventCallback *callback = nullptr, UdpSocketPtr ptr = UdpSocketPtr());

  UdpSocketWithCounter(UdpSocketWithCounter &&);

  virtual ~UdpSocketWithCounter();

  UdpSocketWithCounter& operator=(UdpSocketWithCounter &&rhs);

  UdpSocket* GetUdpSocket() const;

  std::string GetStatisticsInJsonFormat(int64_t ts) const;
  std::string GetStatisticsInPlainText(int64_t ts) const;

 private:
  virtual void OnPacketReceived(
      UdpSocket *skt, const GeneralSocketAddress &local,
      const GeneralSocketAddress &remote, base::Unpacker *,
      uint16_t service_type, uint16_t uri) OVERRIDE;

  virtual void OnPacketSent(
      UdpSocket *impl, const GeneralSocketAddress &from,
      const GeneralSocketAddress &to, uint16_t service_type,
      uint16_t uri, const void *data, size_t len) OVERRIDE;

  virtual void OnWriteReady(UdpSocket *impl) OVERRIDE;

  void MoveFrom(UdpSocketWithCounter *rhs);

 private:
  UdpEventCallback *callback_;

  UdpSocketPtr socket_;
  network::PacketCounterTool counters_;
};

}  // namespace network
}  // namespace agora
