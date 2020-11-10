// Copyright (c) 2014-2019 Agora.io, Inc.
// Fast MemBuf for accerleration
//

#include "media_server_library/memory/mem_buf.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <new>

namespace agora {
namespace memory {

MemBuf* MemBuf::Create(uint32_t capacity) {
  // FIXME(liuyong): Mem pool
  auto p = malloc(sizeof(MemBuf) + capacity);
  return new (p)MemBuf(capacity);
}

MemBuf* MemBuf::Create(const void *buf, uint32_t capacity) {
  // FIXME(liuyong): Mem pool
  auto p = malloc(sizeof(MemBuf) + capacity);
  return new (p)MemBuf(buf, capacity);
}

MemBuf* MemBuf::Create(const void *buf, uint32_t size, uint32_t capacity) {
  assert(size <= capacity);

  // FIXME(liuyong): Mem pool
  auto p = malloc(sizeof(MemBuf) + capacity);
  return new (p)MemBuf(buf, size, capacity);
}

MemBuf::MemBuf(const void *buf, uint32_t capacity) {
  ref_cnt_ = 0;
  capacity_ = capacity;
  ::memcpy(Begin(), buf, capacity);
}

MemBuf::MemBuf(const void *buf, uint32_t size, uint32_t capacity) {
  ref_cnt_ = 0;
  capacity_ = capacity;
  ::memcpy(Begin(), buf, (std::min)(size, capacity));
}

MemBuf::~MemBuf() {
}

MemBuf* MemBuf::Clone() const {
  auto p = MemBuf::Create(capacity_);
  ::memcpy(p->Begin(), Begin(), capacity_);
  return p;
}

}  // namespace memory
}  // namespace agora
