// Copyright (c) 2014-2017 Agora.io, Inc.
// A base class helping to manage sending & receiving data from/to a tcp
// connection with an Report Edge Server
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <memory>

#include "msgpack.hpp"

#include "media_server_library/network/address_types.h"
#include "media_server_library/base/cpp_feature.h"
#include "media_server_library/base/timer.h"

#include "media_server_library/network/report_tcp_connection.h"

namespace io {
namespace agora {
namespace media {
namespace argus {

class ReportItem;

}  // namespace argus
}  // namespace media
}  // namespace agora
}  // namespace io

namespace agora {
namespace network {

class ReportClient;

struct ReportClientCallback {
  virtual void OnConnect(ReportClient *conn, bool connected) = 0;
  virtual void OnPacketReceived(
      ReportClient *conn, const void *buf, size_t length) = 0;

  virtual void OnSocketError(ReportClient *conn) = 0;
  virtual void TimeToPing(ReportClient *conn) = 0;

 protected:
  virtual ~ReportClientCallback() {}
};

class ReportClient : private ReportTcpConnectionCallback,
                    private base::NonCopyOrMovable {
 public:
  ReportClient(event_base *base, const network::SocketAddress4 &remote,
              ReportClientCallback *callback, uint16_t local_port = 0);

  virtual ~ReportClient();

  bool IsConnected() const;
  network::SocketAddress4 GetRemoteAddress() const;
  network::SocketAddress4 GetLocalAddress() const;

  void SetPingInterval(uint32_t seconds);
  void SetTimeout(uint32_t seconds);

  void RegisterCallback(ReportClientCallback *callback);

  bool Connect();
  void Close();

  bool SendPacket(const void *data, uint32_t length,
                  PacketType type, AckMode ack);

  bool SendRawData(const void *data, uint32_t length);

 private:
  struct BufferEventDeleter {
    void operator()(::bufferevent *e) const;
  };

  struct ConnectionDeleter {
    void operator()(ReportTcpConnection *c) const;
  };

  void OnConnect(int events);
  void OnTimer();

  // Inherited from |ReportTcpConnectionCallback|
  virtual void OnError(ReportTcpConnection *conn, int events) OVERRIDE;
  virtual void OnPacketReceived(
      ReportTcpConnection *conn, const void *buf, size_t length) OVERRIDE;

 private:
  static void ConnectCallback(
      bufferevent *bev, short events,  // NOLINT(runtime/int)
      void *context);

 private:
  event_base *base_;
  network::SocketAddress4 addr_;
  ReportClientCallback *callback_;
  base::RepeatedTimer timer_;

  uint16_t local_port_;

  std::unique_ptr<bufferevent, BufferEventDeleter> pending_;
  std::unique_ptr<ReportTcpConnection, ConnectionDeleter> conn_;

  bool connecting_;
  bool stopped_;

  int32_t alive_interval_;
  int32_t last_active_ts_;
};

}  // namespace network
}  // namespace agora
