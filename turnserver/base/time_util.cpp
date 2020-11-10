// Copyright (c) 2014-2017 Agora.io, Inc.
// TODO(liuyong): requires a detailed description
//

#include "base/time_util.h"

#include <dlfcn.h>
#include <sys/time.h>
#include <time.h>

namespace agora {
namespace base {

typedef int clock_gettime_func(clockid_t, struct timespec *);
typedef int gettimeofday_func(struct timeval *, __timezone_ptr_t);

namespace {

// NOTE(liuyong): Fixes a glibc 2.19 and belower performance issue where
// clock_gettime under MONOTONIC mode always invokes a system call.
// Many thanks to Lionfore Hao.
static clock_gettime_func* GetClockGetTimeFunc() {
  clock_gettime_func *f = clock_gettime;

#if defined(__i386__) && !defined(STATIC_LINK)
  auto h_vdso = dlopen("linux-gate.so.1", RTLD_NOW);
  if (!h_vdso) {
    return f;
  }

  auto sym = reinterpret_cast<clock_gettime_func *>(
      dlsym(h_vdso, "__vdso_clock_gettime"));
  if (sym) {
    f = sym;
  }

  dlclose(h_vdso);
#endif

  return f;
}

static gettimeofday_func* GetTimeOfDayFunc() {
  gettimeofday_func *f = gettimeofday;

#if defined(__i386__) && !defined(STATIC_LINK)
  auto h_vdso = dlopen("linux-gate.so.1", RTLD_NOW);
  if (!h_vdso) {
    return f;
  }

  auto sym = reinterpret_cast<gettimeofday_func *>(
      dlsym(h_vdso, "__vdso_gettimeofday"));
  if (sym) {
    f = sym;
  }

  dlclose(h_vdso);
#endif

  return f;
}

}  // namespace

int64_t TickInMicroSeconds() {
  timespec spec = {0, 0};

  static clock_gettime_func *const clock_func = GetClockGetTimeFunc();

  clock_func(CLOCK_MONOTONIC_COARSE, &spec);
  return spec.tv_sec * 1000ll * 1000 + (spec.tv_nsec / 1000ll);
}

int64_t TickInMilliSeconds() {
  return TickInMicroSeconds() / 1000;
}

int32_t TickInSeconds() {
  return static_cast<int32_t>(TickInMilliSeconds() / 1000);
}

int64_t HighResolutionTickInMicroSeconds() {
  timespec spec = {0, 0};

  static clock_gettime_func *const clock_func = GetClockGetTimeFunc();

  clock_func(CLOCK_MONOTONIC, &spec);
  return spec.tv_sec * 1000ll * 1000 + (spec.tv_nsec / 1000ll);
}

int64_t HighResolutionTickInMilliSeconds() {
  return HighResolutionTickInMicroSeconds() / 1000;
}

int32_t HighResolutionTickInSeconds() {
  return static_cast<int32_t>(HighResolutionTickInMilliSeconds() / 1000);
}

int64_t TickMicroSeconds() {
  return TickInMicroSeconds();
}

int64_t TickMilliSeconds() {
  return TickInMilliSeconds();
}

int32_t TickSeconds() {
  return TickInSeconds();
}

int64_t NowInMicroSeconds() {
  timeval t = {0, 0};

  static gettimeofday_func *const get_time = GetTimeOfDayFunc();
  get_time(&t, nullptr);

  return (int64_t)(t.tv_sec) * 1000llu * 1000 + t.tv_usec;
}

int64_t NowInMilliSeconds() {
  return NowInMicroSeconds() / 1000;
}

int32_t NowInSeconds() {
  return static_cast<int32_t>(NowInMilliSeconds() / 1000);
}

}  // namespace base
}  // namespace agora
