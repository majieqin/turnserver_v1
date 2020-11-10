// Copyright (c) 2014-2017 Agora.io, Inc.
// A base class for allocating a port through which all udp packets send/receive
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <deque>
#include <memory>
#include <vector>

#include "event2/event.h"

#include "media_server_library/base/cpp_feature.h"
#include "media_server_library/base/timer.h"

#include "media_server_library/network/address_types.h"

struct event_base;

namespace agora {
namespace network {

// FIXME(liuyong): Documents here
enum class FlushMode : uint32_t {
  kDropFailed = 0,
  kNoDropped = 1,
};

class UdpSocketImpl;

struct UdpCallbackInternal {
  virtual void OnPacketReceived(
      UdpSocketImpl *impl, const sockaddr_storage &remote, const void *data,
      size_t len) = 0;

  virtual void OnPacketSent(
      UdpSocketImpl *impl, const sockaddr_storage &remote, const void *data,
      size_t len);

  virtual void OnWriteReady(UdpSocketImpl *impl);

 protected:
  virtual ~UdpCallbackInternal();
};

class UdpSocketImpl : private base::NonCopyOrMovable {
 public:
  static UdpSocketImpl* Create(
      event_base *loop, int fd, size_t send_queue_size = 4096,
      UdpCallbackInternal *listener = nullptr);

  void Destroy();

  int Detach();
  UdpCallbackInternal* RegisterCallback(UdpCallbackInternal *c);

  void SetAutoFlushInterval(int interval_ms = 10);
  // void AdjustUdpBuffer(int udp_size);

  int SendData(const network::NewSockAddress4 &remote,
               std::vector<char> buffer);

  int SendData(const network::SocketAddress6 &remote, std::vector<char> buffer);
  int Flush(FlushMode mode = FlushMode::kDropFailed);

 private:
  UdpSocketImpl(event_base *loop, int fd, size_t cache_size,
                UdpCallbackInternal *observer);

  ~UdpSocketImpl();

  struct EventDeleter {
    void operator()(::event *e) const {
      if (e) {
        ::event_del(e);
        ::event_free(e);
      }
    }
  };

  struct UdpMessage;

  int SendDataInternal(
      const sockaddr_storage &remote, std::vector<char> buffer);

  bool SendImmediately(
      const sockaddr_storage &remote, const std::vector<char> &buffer);

  void DestroyInternal();

  void OnTimer();
  bool SetupCallback(int flags);
  void HandleEvents(int events);
  void OnDataReady();

  // NOLINTNEXTLINE(runtime/int)
  static void EventCallback(int fd, short events, void *context);

 private:
  event_base *loop_;
  UdpCallbackInternal *callback_;
  int fd_;
  FlushMode flush_mode_;

  bool closed_;
  bool processing_;

  base::RepeatedTimer timer_;
  std::unique_ptr<event, EventDeleter> event_;

  // for buckly sending multiple messages
  const size_t cache_size_;

  std::deque<UdpMessage> buffers_to_send_;
};

}  // namespace network
}  // namespace agora
