// Copyright (c) 2014-2017 Agora.io, Inc.
// A base class helping to manage sending & receiving data from/to a tcp socket
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>

#include <event2/buffer.h>  // NOLINT(build/include_order)
#include <event2/bufferevent.h>  // NOLINT(build/include_order)
#include <event2/event.h>  // NOLINT(build/include_order)
#include <event2/listener.h>  // NOLINT(build/include_order)

#include "network/address_types.h"

namespace agora {
namespace base {

class EventLoop;

}  // namespace base

namespace network {

class GenericTcpConnection {
 public:
  class Visitor {
   public:
    virtual void OnError(
        GenericTcpConnection *conn, int events) = 0;
    virtual void OnDataReceived(
        GenericTcpConnection *conn, const void *buffer, uint32_t length) = 0;
   protected:
    virtual ~Visitor() = default;
  };

  enum LogMode {
    kVerbose = 0,
    kSilent = 1,
  };

  static GenericTcpConnection* Create(
      base::EventLoop *loop, int fd, LogMode mode = kVerbose);

  void RegisterCallback(Visitor *visitor);

  base::EventLoop* GetEventLoop() const;

  void SendData(const void *buffer, uint32_t length);

  int Detach();

  void Flush();

  void CloseAndDestroy();

  bool IsClosed() const;
  network::SocketAddress4 GetRemoteAddress() const;
  network::SocketAddress4 GetLocalAddress() const;

 private:
  GenericTcpConnection(base::EventLoop *loop, int fd, LogMode mode);

  ~GenericTcpConnection();

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
  base::EventLoop *loop_;
  Visitor *visitor_;
  bufferevent *bev_;

  LogMode mode_;
  bool is_in_processing_;

  network::SocketAddress4 remote_;
  network::SocketAddress4 local_;
};

inline bool GenericTcpConnection::IsClosed() const {
  return bev_ == nullptr;
}

inline network::SocketAddress4 GenericTcpConnection::GetRemoteAddress() const {
  return remote_;
}

inline network::SocketAddress4 GenericTcpConnection::GetLocalAddress() const {
  return local_;
}

inline base::EventLoop* GenericTcpConnection::GetEventLoop() const {
  return loop_;
}

}  // namespace network
}  // namespace agora
