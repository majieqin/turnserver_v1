// Copyright (c) 2014-2017 Agora.io, Inc.
// A base class helping to manage sending & receiving data from/to a tcp
// connection with an Report Edge Server
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>

#include "event2/buffer.h"  // NOLINT(build/include_order)
#include "event2/bufferevent.h"  // NOLINT(build/include_order)
#include "event2/event.h"  // NOLINT(build/include_order)
#include "event2/listener.h"  // NOLINT(build/include_order)

#include "media_server_library/base/common_types.h"
#include "media_server_library/network/address_types.h"

namespace agora {
namespace network {

enum PacketType {
  kMsgPack = 0,
  kThrift = 2,
  kProtoBuffer = 4,
};

enum AckMode {
  kNoAckRequired = 0,
  kAckRequired = 1,
  kAckDebug = 101,
};

class ReportTcpConnection;

struct ReportTcpConnectionCallback {
  // This connection was closed by the remote peer.
  virtual void OnError(ReportTcpConnection *conn, int events) = 0;

  virtual void OnPacketReceived(
      ReportTcpConnection *conn, const void *buf, size_t length) = 0;

 protected:
  virtual ~ReportTcpConnectionCallback() {}
};

class ReportTcpConnection {
 public:
  static ReportTcpConnection* Create(event_base *base, evutil_socket_t fd);
  static ReportTcpConnection* Create(event_base *base, bufferevent *bev);

  network::SocketAddress4 GetRemoteAddress() const;
  network::SocketAddress4 GetLocalAddress() const;

  bool IsClosed() const;
  event_base* GetEventBase() const;

  void RegisterCallback(ReportTcpConnectionCallback *callback);

  // close the socket and destroy this connection object
  void CloseAndDestroy();

  bool SendRawData(const void *data, uint32_t length);

  bool SendPacket(const void *data, uint32_t length, PacketType type,
                  AckMode ack);

  // Returns the underlying fd and detachs from it.
  int Detach();

 private:
  ReportTcpConnection(event_base *base, evutil_socket_t fd);
  ReportTcpConnection(event_base *base, bufferevent *bev);
  ~ReportTcpConnection();

  void Close();
  void Destroy();

  void SetSocketFd(evutil_socket_t fd);
  void FillSocketAddress(evutil_socket_t fd);
  void OnDataReady();
  void OnEvent(int events);

 private:
  static void ReadCallback(bufferevent *bev, void *context);
  static void WriteCallback(bufferevent *bev, void *context);
  static void EventCallback(bufferevent *bev,
                            short events,  // NOLINT(runtime/int)
                            void *context);

 private:
  bool is_in_processing_;

  event_base *base_;
  bufferevent *bev_;
  ReportTcpConnectionCallback *callback_;

  network::SocketAddress4 remote_;
  network::SocketAddress4 local_;
};

inline bool ReportTcpConnection::IsClosed() const {
  return bev_ == nullptr;
}

inline event_base* ReportTcpConnection::GetEventBase() const {
  return base_;
}

}  // namespace network
}  // namespace agora
