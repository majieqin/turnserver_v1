// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
//
// TODO(liuyong): A more detailed description is required.

#include "base/timer.h"

#include <event2/event.h>
#include <event2/event_struct.h>

#include <utility>
#include <iostream>

namespace agora {
namespace base {

RepeatedTimer::RepeatedTimer() {
  timer_ = nullptr;
}

RepeatedTimer::~RepeatedTimer() {
  Clear();
}

RepeatedTimer::RepeatedTimer(RepeatedTimer &&rhs) {
  timer_ = nullptr;

  std::swap(timer_, rhs.timer_);
  callback_.swap(rhs.callback_);
}

RepeatedTimer& RepeatedTimer::operator=(RepeatedTimer &&rhs) {
  std::swap(timer_, rhs.timer_);
  callback_.swap(rhs.callback_);

  return *this;
}

void RepeatedTimer::TimerCallback(int fd, short events,  // NOLINT(runtime/int)
                                  void *context) {
  (void)fd;
  (void)events;

  TimerBase *callback = reinterpret_cast<TimerBase *>(context);
  callback->OnTimer();
}

void RepeatedTimer::CreateTimer(event_base *loop, uint32_t interval_ms) {
  timeval t;
  std::cout << "RepeatedTimer::CreateTimer1" << std::endl;
  t.tv_sec = interval_ms / 1000;
  t.tv_usec = (interval_ms % 1000) * 1000ull;

  std::cout << "RepeatedTimer::CreateTimer2" << std::endl;
  timer_ = ::event_new(loop, -1, EV_READ | EV_PERSIST,
                       RepeatedTimer::TimerCallback, callback_.get());
  std::cout << "RepeatedTimer::CreateTimer3" << std::endl;
  ::evtimer_add(timer_, &t);
  std::cout << "RepeatedTimer::CreateTimer4" << std::endl;
}

void RepeatedTimer::Clear() {
  if (timer_) {
    ::evtimer_del(timer_);
    ::event_free(timer_);
  }

  timer_ = nullptr;
}

void OneShotTimer::EventDeleter::operator()(event *e) const {
  if (e) {
    ::evtimer_del(e);
    ::event_free(e);
  }
}

OneShotTimer::OneShotTimer(
    event_base *loop, uint32_t after_ms, std::unique_ptr<TimerBase> callback) {
  callback_ = std::move(callback);

  CreateTimerInternal(loop, after_ms);
}

OneShotTimer::~OneShotTimer() {
}

void OneShotTimer::CreateTimerInternal(event_base *loop, uint32_t after_ms) {
  timeval t;
  t.tv_sec = after_ms / 1000;
  t.tv_usec = (after_ms % 1000) * 1000ull;

  auto timer = ::event_new(loop, -1, EV_READ | EV_PERSIST,
                           OneShotTimer::TimerCallback, this);

  ::evtimer_add(timer, &t);
  timer_.reset(timer);
}

void OneShotTimer::TimerCallback(int fd, short events,  // NOLINT(runtime/int)
                                 void *context) {
  (void)fd;
  (void)events;

  auto p = reinterpret_cast<OneShotTimer *>(context);
  p->callback_->OnTimer();

  delete p;
}

}  // namespace base
}  // namespace agora
