// Copyright (c) 2014-2017 Agora.io, Inc.
// A tcp connection class to read/write lines from/to a tcp socket
//

#pragma once  // NOLINT(build/header_guard)

#include <netinet/in.h>

#include <string>

#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/event.h"
#include "event2/listener.h"

#include "media_server_library/network/address_types.h"

namespace agora {
namespace network {

class LineBufferedConnection;

struct LineHandler {
  // Received a line from |c|. The line ending is removed.
  virtual void OnLineReceived(const std::string &line,
                              LineBufferedConnection *c) = 0;

  virtual void OnError(int events, LineBufferedConnection *c) = 0;

 protected:
  virtual ~LineHandler() {}
};

class LineBufferedConnection {
 public:
  static LineBufferedConnection* Create(event_base *base, int fd = -1);

  network::SocketAddress4 GetRemoteAddress() const;
  network::SocketAddress4 GetLocalAddress() const;

  bool IsClosed() const;
  event_base* GetEventBase() const;

  void SetLineHandler(LineHandler *handler);

  // Closes the socket and destroys this connection object
  void CloseAndDestroy();
  void Close();
  void Flush();

  // Sends a string to the remote peer. |content| may be composed of multiple
  // lines. No CR/LF is appended to the end of |content|.
  void SendString(const std::string &content);

  // Sends a raw buffer to the remote peer.
  void SendRawBuffer(const void *data, uint32_t length);

 private:
  LineBufferedConnection(event_base *base, int fd);
  ~LineBufferedConnection();

  void SetSocketFd(evutil_socket_t fd);
  void FillSocketAddress(evutil_socket_t fd);
  void OnDataReady();
  void ProcessLines();

  // Processes the current line in |cur_line|
  void HandleSingleLine();

  void OnEvent(int events);
  void Destroy();

 private:
  static void ReadCallback(bufferevent *bev, void *context);  // NOLINT
  static void WriteCallback(bufferevent *bev, void *context);  // NOLINT
  static void EventCallback(bufferevent *bev, short events,  // NOLINT
                            void *context);  // NOLINT

 private:
  bool is_in_processing_;

  event_base *base_;
  bufferevent *bev_;
  LineHandler *handler_;

  network::SocketAddress4 remote_;
  network::SocketAddress4 local_;

  std::string buffer_;
  std::string cur_line_;
};

inline bool LineBufferedConnection::IsClosed() const {
  return bev_ == NULL;
}

inline event_base* LineBufferedConnection::GetEventBase() const {
  return base_;
}

inline
network::SocketAddress4 LineBufferedConnection::GetRemoteAddress() const {
  return remote_;
}

inline network::SocketAddress4 LineBufferedConnection::GetLocalAddress() const {
  return local_;
}

}  // namespace network
}  // namespace agora
