// Copyright (c) 2014-2017 Agora.io, Inc.
// A base class for implementing high-qps low-traffic TCP Connection
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <cstdio>
#include <memory>
#include <queue>
#include <vector>

#include "event2/event.h"

#include "media_server_library/base/cpp_feature.h"
#include "media_server_library/base/event_loop.h"
#include "media_server_library/base/packet.h"
#include "media_server_library/base/unpacker.h"

#include "media_server_library/io/fd_buffer.h"

#include "media_server_library/memory/mem_slice_span.h"
#include "media_server_library/network/address_types.h"

namespace agora {
namespace base {
class EventLoop;
}  // namespace base

namespace network {

class FastTcpConnection;
class TcpConnection;

enum ErrorType {
  kEventReading = 0x01,
  kEventWriting = 0x02,
  kEventIllegalPacket = 0x04,
  kEventEof = 0x10,
  kEventError = 0x20,
};

enum {
  kSlicePerCommit = 128,
};

class FastTcpConnection : private base::NonCopyOrMovable {
 public:
  enum {
    kBufferSizeUnspecified = 0,
    kMinimumCacheSize = 4096,
    kMaximumCacheSize = 256 * 1024 * 1024,  // 512M
  };

  enum class CacheMode : uint32_t {
    kDirect = 1,
    kBuffered = 2,
  };

  struct Visitor;

  static FastTcpConnection* Create(
      event_base *base, int fd, Visitor *callback,
      uint32_t buffer_size = kBufferSizeUnspecified,
      CacheMode cache_mode = CacheMode::kDirect);

  static FastTcpConnection* Create(
      base::EventLoop *loop, int fd, Visitor *callback,
      uint32_t buffer_size = kBufferSizeUnspecified,
      CacheMode cache_mode = CacheMode::kDirect);

  // Converts from |TcpConnection|
  // After called, |*c| will be destroyed
  static FastTcpConnection* Create(
      TcpConnection *c, Visitor *callback,
      uint32_t buffer_size = kBufferSizeUnspecified,
      CacheMode cache_mode = CacheMode::kDirect);

  int GetFd() const;
  network::NewSockAddress4 GetRemoteAddress() const;
  network::NewSockAddress4 GetLocalAddress() const;

  ::event_base* GetEventBase() const;

  void SetCacheMode(CacheMode cache_mode, uint32_t cache_size);
  void SetMaximumCacheSize(uint32_t cache_size);
  bool EnableNoDelay(bool enable);

  // NOTE(liuyong): Adjust socket snd/rcv buffer in kernel
  bool SetSocketBufferSize(uint32_t size);

  void RegisterCallback(Visitor *callback);
  bool CloseAndDestroy();

  enum class ResultType {
    kSent = 0,
    kPending = 1,
    kBroken = 2,
    kBufOverflow = 3,
  };

  // NOTE(liuyong):
  // Return value:
  // 1) |kSent| means the buffer is delivered to the system successfully,
  // 2) |kPending|, the buffer is held in the user side and waiting for |Flush|
  //    or a writable event triggered.
  // 3) |kBroken| means the socket is broken. You should close and destroy
  //    the connection. No |OnError| is called
  // 4) |kBufOverflow| means the pending buffer is too large, this send
  //    operation is neglected.
  ResultType SendBuffer(memory::MemSlice buffer);
  ResultType SendBuffer(memory::MemSliceSpan buffer);
  ResultType SendBuffer(const std::vector<char> &buffer);
  ResultType SendPacket(const base::Packet &packet);

  bool Flush();

 private:
  FastTcpConnection(base::EventLoop *base, int fd, Visitor *callback,
                    uint32_t buffer_size, CacheMode cache_mode);

  FastTcpConnection(event_base *base, int fd, Visitor *callback,
                    uint32_t buffer_size, CacheMode cache_mode);

  ~FastTcpConnection();

  bool IsClosed() const;

  bool SetupCallback(int flags);
  void DisableCallback();

  void FillSocketAddress();

  bool ReadPacketHead();
  bool ReadInternal();

  enum class CallerMode {
    kBySystem = 0,
    kByClient = 1,
  };

  ResultType SendBufferInternal(CallerMode caller);

  bool FlushBufferInternal(uint32_t index, uint32_t *sent, bool *unrecoverable);

  // delete this
  void Destroy();

  // Releases resources
  void Release();

  void HandleEvents(int events);

  // NOLINTNEXTLINE(runtime/int)
  static void EventCallback(int fd, short events, void *context);

 private:
  event_base *loop_;
  Visitor *callback_;
  int fd_;

  network::NewSockAddress4 remote_;
  network::NewSockAddress4 local_;

  // Beginning of variables related to read operations
  std::unique_ptr<io::FdBuffer> read_buffer_;
  bool closed_;
  bool in_read_;
  bool in_write_;

  // A partial packet waiting for more data received.
  uint32_t packet_size_;
  uint32_t readed_;
  std::unique_ptr<char[]> buffer_;
  // End

  bool writable_;

  CacheMode cache_mode_;

  // Threshold for flush
  uint32_t cache_size_;
  uint32_t pending_size_;

  // maximum sending buffer size in user mode
  uint32_t max_cache_size_;

  std::vector<memory::MemSlice> buffers_to_send_;

  std::unique_ptr<event, base::internal::EventDeleter> event_;
};

struct FastTcpConnection::Visitor {
  virtual void OnPacketReceived(FastTcpConnection *c, base::Unpacker *pkr,
                                uint16_t service_type, uint16_t uri) = 0;

  virtual void OnError(FastTcpConnection *c, int events) = 0;

 protected:
  virtual ~Visitor();
};

typedef FastTcpConnection::Visitor FastTcpConnectionCallback;

inline network::NewSockAddress4 FastTcpConnection::GetRemoteAddress() const {
  return remote_;
}

inline network::NewSockAddress4 FastTcpConnection::GetLocalAddress() const {
  return local_;
}

inline ::event_base* FastTcpConnection::GetEventBase() const {
  return loop_;
}

struct ConnectionDeleter {
  void operator()(FastTcpConnection *c) const {
    c->CloseAndDestroy();
  }
};

typedef std::unique_ptr<FastTcpConnection,
                        ConnectionDeleter> FastTcpConnectionPtr;

}  // namespace network
}  // namespace agora
