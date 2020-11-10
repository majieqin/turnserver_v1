// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
//
// TODO(liuyong): A more detailed description is required.

#pragma once  // NOLINT(build/header_guard)

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>

#include "cpp_feature.h"
#include "event_loop.h"

struct event_base;
struct event;

namespace agora {
namespace base {

class RepeatedTimer {
 public:
  RepeatedTimer();

  template <typename Callable, typename... Args>
  RepeatedTimer(event_base *loop, uint32_t interval_ms, Callable &&f,
                Args&&... args);

  template <typename Callable, typename... Args>
  RepeatedTimer(EventLoop *loop, uint32_t interval_ms, Callable &&f,
                Args&&... args);

  template <typename Callable, typename... Args>
  RepeatedTimer(EventLoop *loop, std::chrono::milliseconds interval,
                Callable &&f, Args&&... args);

  RepeatedTimer(RepeatedTimer &&);
  RepeatedTimer& operator=(RepeatedTimer &&);

  ~RepeatedTimer();

  bool Empty() const;

  // Destroys the timer
  void Clear();

 private:
  struct TimerBase {
    virtual ~TimerBase() {}
    virtual void OnTimer() = 0;
  };

  template <typename Callable>
  struct TimerImpl : public TimerBase {
    explicit TimerImpl(Callable &&f) : f_(std::forward<Callable>(f)) {}
    virtual ~TimerImpl() {}

    virtual void OnTimer() { f_(); }

   private:
    Callable f_;
  };

  template <typename Callable>
  static std::unique_ptr<TimerImpl<Callable> > InternalMakeImpl(Callable &&f) {
    typedef TimerImpl<Callable> Implement;
    return std::unique_ptr<Implement>(new Implement(std::forward<Callable>(f)));
  }

  RepeatedTimer(const RepeatedTimer &) = delete;
  RepeatedTimer& operator=(const RepeatedTimer &) = delete;

  void CreateTimer(event_base *loop, uint32_t interval_ms);

  static void TimerCallback(int fd, short events,  // NOLINT(runtime/int)
                            void *context);

 private:
  event *timer_;
  std::unique_ptr<TimerBase> callback_;
};

template <typename Callable, typename... Args>
RepeatedTimer::RepeatedTimer(event_base *loop, uint32_t interval_ms,
                             Callable &&f, Args&&... args) {
  callback_ = InternalMakeImpl(std::bind<void>(std::forward<Callable>(f),
                                               std::forward<Args>(args)...));
  CreateTimer(loop, interval_ms);
}

template <typename Callable, typename... Args>
RepeatedTimer::RepeatedTimer(EventLoop *loop, uint32_t interval_ms,
                             Callable &&f, Args&&... args)
    : RepeatedTimer(loop->GetEventBase(), interval_ms,
                    std::forward<Callable>(f),
                    std::forward<Args>(args)...) {
}

template <typename Callable, typename... Args>
RepeatedTimer::RepeatedTimer(EventLoop *loop,
                             std::chrono::milliseconds interval,
                             Callable &&f, Args&&... args)
    : RepeatedTimer(loop->GetEventBase(),
                    std::max<uint32_t>(1u, interval.count()),
                    std::forward<Callable>(f),
                    std::forward<Args>(args)...) {
}

inline bool RepeatedTimer::Empty() const {
  return timer_ == nullptr;
}

class OneShotTimer : private NonCopyOrMovable {
 public:
  template <typename Callable, typename... Args>
  static void MakeOneShotTimer(
      base::EventLoop *loop, uint32_t after_ms, Callable &&f, Args&&... args);

  template <typename Callable, typename... Args>
  static void MakeOneShotTimer(
      event_base *loop, uint32_t after_ms, Callable &&f, Args&&... args);

 private:
  struct TimerBase {
    virtual ~TimerBase() {}
    virtual void OnTimer() = 0;
  };

  template <typename Callable>
  struct TimerImpl : public TimerBase {
    explicit TimerImpl(Callable &&f)
        : f_(std::forward<Callable>(f)) {
    }

    virtual ~TimerImpl() {}

    virtual void OnTimer() {
      f_();
    }

   private:
    Callable f_;
  };

  template <typename Callable>
  static std::unique_ptr<TimerImpl<Callable> > InternalMakeImpl(Callable &&f) {
    typedef TimerImpl<Callable> Implement;
    return std::unique_ptr<Implement>(new Implement(std::forward<Callable>(f)));
  }

  OneShotTimer(
      event_base *loop, uint32_t after_ms, std::unique_ptr<TimerBase> callback);

  ~OneShotTimer();

  void CreateTimerInternal(event_base *loop, uint32_t interval_ms);

  static void TimerCallback(
      int fd, short events,  // NOLINT(runtime/int)
      void *context);

 private:
  struct EventDeleter {
    void operator()(event *e) const;
  };

  std::unique_ptr<event, EventDeleter> timer_;
  std::unique_ptr<TimerBase> callback_;
};

template <typename Callable, typename... Args>
void OneShotTimer::MakeOneShotTimer(
    base::EventLoop *loop, uint32_t after_ms, Callable &&f, Args&&... args) {
  OneShotTimer::MakeOneShotTimer(loop->GetEventBase(), after_ms,
                                 std::forward<Callable>(f),
                                 std::forward<Args>(args)...);
}

template <typename Callable, typename... Args>
void OneShotTimer::MakeOneShotTimer(
    event_base *loop, uint32_t after_ms, Callable &&f, Args&&... args) {
  auto callback = InternalMakeImpl(
      std::bind<void>(std::forward<Callable>(f), std::forward<Args>(args)...));

  (void)new OneShotTimer(loop, after_ms, std::move(callback));
}

}  // namespace base
}  // namespace agora
