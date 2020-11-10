// Copyright (c) 2014-2017 Agora.io, Inc.
// Basic utilities for preventing error-prone source codes.
//

#pragma once  // NOLINT(build/header_guard)

namespace agora {
namespace base {

class NonCopyable {
 protected:
  NonCopyable() {}
  ~NonCopyable() {}

 private:
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable& operator=(const NonCopyable &) = delete;
};

class NonMovable {
 protected:
  NonMovable() {}
  ~NonMovable() {}

 private:
  NonMovable(NonMovable &&) = delete;
  NonMovable& operator=(NonMovable &&) = delete;
};

class NonCopyOrMovable {
 protected:
  NonCopyOrMovable() {}
  ~NonCopyOrMovable() {}

 private:
  NonCopyOrMovable(const NonCopyOrMovable &) = delete;
  NonCopyOrMovable(NonCopyOrMovable &&) = delete;
  NonCopyOrMovable& operator=(const NonCopyOrMovable &) = delete;
  NonCopyOrMovable& operator=(NonCopyOrMovable &&) = delete;
};

#if __cplusplus >= 20110ll
#define OVERRIDE override
#define FINAL final
#else
#define OVERRIDE
#define FINAL
#endif

// unused mark
#if __cplusplus >= 20110ll
#define MAYBE_UNUSED [[gnu::unused]]  // NOLINT
#else
#define MAYBE_UNUSED __attribute__((unused))
#endif

// branch predict
#if __cplusplus >= 20110ll
#define PREFER_TRUE(cond)  __builtin_expect(static_cast<bool>(cond), 1)
#define PREFER_FALSE(cond) __builtin_expect(static_cast<bool>(cond), 0)
#else
#define PREFER_TRUE(cond)  __builtin_expect(!!(cond), 1)
#define PREFER_FALSE(cond) __builtin_expect(!!(cond), 0)
#endif

#define ALWAYS_INLINE inline __attribute__((always_inline))

}  // namespace base
}  // namespace agora
