// Copyright (c) 2014-2017 Agora.io, Inc.
// A console helper class to facilize interacting with a long-run server.
//

#pragma once  // NOLINT(build/header_guard)

#include <netinet/in.h>

#include <cstdint>
#include <string>
#include <unordered_map>

#include "event2/event.h"
#include "event2/listener.h"

#include "media_server_library/base/cpp_feature.h"
#include "media_server_library/network/generic_tcp_server.h"
#include "media_server_library/network/line_buffered_connection.h"

namespace agora {
namespace network {

struct CommandHandler {
  // Received a line from a client. The line ending is removed.
  // Returns false, if you want to close the connection.
  virtual bool OnCommandLine(const std::string &line,
                             std::string *response) = 0;

 protected:
  virtual ~CommandHandler() {}
};

class ConsoleAgent : private GenericTcpServerCallback, private LineHandler {
 public:
  ConsoleAgent(base::EventLoop *loop, CommandHandler *handler);
  ConsoleAgent(event_base *loop, CommandHandler *handler);
  virtual ~ConsoleAgent();

  // Machine-ordered
  bool ListenOn(uint16_t port, uint32_t local_address = INADDR_LOOPBACK);

 private:
  virtual void OnConnectionAccepted(
      int fd, const network::GeneralSocketAddress &remote_addr) OVERRIDE;

  virtual void OnLineReceived(const std::string &line,
                              LineBufferedConnection *c) OVERRIDE;

  virtual void OnError(int events, LineBufferedConnection *c) OVERRIDE;

 private:
  event_base *base_;
  CommandHandler *handler_;

  GenericTcpServer monitor_;
};

}  // namespace network
}  // namespace agora
