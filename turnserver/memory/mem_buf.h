// Copyright (c) 2014-2019 Agora.io, Inc.
// Fast MemBuf for accerleration
//

#pragma once  // NOLINT(build/header_guard)

#include <atomic>
#include <cassert>
#include <cstdlib>
#include <memory>

#include "../base/cpp_feature.h"

namespace agora {
namespace memory {

// NOT Thread-safe
// Memory alignment
//                   ----------- <--------- object
//                   | ref_cnt |
//                   |---------|
//                   | capacity|
//             |     |---------|<------|---- Buffer
//             |     |         |       |
//             |---->|---------|       |
//                   |         |       |
//                   |         |       |
//                   |         |       |
//                   |---------|<------|
//                   |         |
//                   -----------
class MemBuf : private base::NonCopyOrMovable {
 public:
  enum { kDefaultPacketSize = 2000, };

  static MemBuf* Create(uint32_t size = kDefaultPacketSize);
  static MemBuf* Create(const void *buf, uint32_t size = kDefaultPacketSize);
  static MemBuf* Create(const void *buf, uint32_t size, uint32_t capacity);

  // NOTE(liuyong): ugly implementation.
  void AddRef() const;
  void Release() const;
  int32_t RefCount() const;
  const void* Begin() const;
  const void* End() const;

  void* Begin();
  void* End();

  MemBuf* Clone() const;
  uint32_t Capacity() const;

 private:
  explicit MemBuf(uint32_t size);
  MemBuf(const void *buf, uint32_t size);
  MemBuf(const void *buf, uint32_t size, uint32_t capacity);
  ~MemBuf();

  mutable std::atomic<int32_t> ref_cnt_;
  uint32_t capacity_;
};

inline int32_t MemBuf::RefCount() const {
  return ref_cnt_.load(std::memory_order_acquire);
}

inline void MemBuf::AddRef() const {
  ref_cnt_.fetch_add(1, std::memory_order_acq_rel);
}

inline void MemBuf::Release() const {
  if (ref_cnt_.fetch_sub(1, std::memory_order_acq_rel) <= 1) {
    this->MemBuf::~MemBuf();
    auto p = const_cast<char *>(reinterpret_cast<const char *>(this));
    free(p);
  }
}

inline const void* MemBuf::Begin() const {
  return reinterpret_cast<const void *>(this + 1);
}

inline const void* MemBuf::End() const {
  return reinterpret_cast<const char *>(this + 1) + capacity_;
}

inline void* MemBuf::Begin() {
  return reinterpret_cast<void *>(this + 1);
}

inline void* MemBuf::End() {
  return reinterpret_cast<char *>(this + 1) + capacity_;
}

inline uint32_t MemBuf::Capacity() const {
  return capacity_;
}

inline MemBuf::MemBuf(uint32_t capacity) {
  ref_cnt_ = 0;
  capacity_ = capacity;
}

}  // namespace memory
}  // namespace agora
