// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
//
// The basic event driver for asyncronously event handling

#pragma once  // NOLINT(build/header_guard)

#include <event2/event.h>

namespace agora {
namespace base {

class EventLoop {
 public:
  EventLoop();
  ~EventLoop();

  // Return value:
  // 0 means success, otherwise, -1
  int Run();

  // Return value:
  // 0 means success, otherwise, -1
  int Stop();

  event_base* GetEventBase() const;

 private:
  bool Initialize();

 private:
  event_base *event_base_;
};

namespace internal {

// NOTE(liuyong): DO NOT use this class. It is aimed to be used in the library
struct EventDeleter {
  void operator()(void *e) const;
};

}  // namespace internal

}  // namespace base
}  // namespace agora
