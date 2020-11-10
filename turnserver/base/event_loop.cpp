// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
//
// Implementations of |EventLoop|

#include "event_loop.h"

#include <csignal>

#include "../logging/safe_log.h"

namespace agora {
namespace base {

EventLoop::EventLoop() {
  if ((event_base_ = ::event_base_new()) == nullptr) {
    SAFE_LOG2(Fatal) << "Failed to create an event_base";
  }
}

EventLoop::~EventLoop() {
  if (event_base_) {
    ::event_base_free(event_base_);
    event_base_ = nullptr;
  }
}

static void LibEventLog(int severity, const char *msg) {
  switch (severity) {
    case EVENT_LOG_DEBUG:
      SAFE_LOG2(Debug) << msg;
      break;
    case EVENT_LOG_MSG:
      SAFE_LOG2(Info) << msg;
      break;
    case EVENT_LOG_WARN:
      SAFE_LOG2(Warn) << msg;
      break;
    case EVENT_LOG_ERR:
    default:
      SAFE_LOG2(Error) << msg;
      break;
  }
}

bool EventLoop::Initialize() {
  if (signal(SIGHUP, SIG_IGN) == SIG_ERR) {
    LOG2(Error, "Ignore SIGHUP failed.");
    return false;
  }

  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    LOG2(Error, "Ignore SIGPIPE failed.");
    return false;
  }

  ::event_set_log_callback(&LibEventLog);
  return true;
}

int EventLoop::Run() {
  if (!Initialize()) {
    return -1;
  }

  return event_base_dispatch(event_base_);
}

int EventLoop::Stop() {
  if (!event_base_)
    return 0;

  return ::event_base_loopexit(event_base_, nullptr);
}

event_base* EventLoop::GetEventBase() const {
  return event_base_;
}

namespace internal {

void EventDeleter::operator()(void *p) const {
  auto e = reinterpret_cast<event *>(p);
  if (e) {
    ::event_del(e);
    ::event_free(e);
  }
}

}  // namespace internal

}  // namespace base
}  // namespace agora
