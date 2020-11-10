// Copyright (c) 2014-2018 Agora.io, Inc.
// Fast MemBuf for accerleration
//

#pragma once  // NOLINT(build/header_guard)

#include <cassert>
#include <initializer_list>

#include "media_server_library/base/common_types.h"
#include "media_server_library/memory/mem_buf.h"
#include "media_server_library/memory/mem_slice.h"

namespace agora {
namespace memory {

// NOTE(liuyong):
// |MemSliceSpan| now is read-only for sending scattered blocks
class MemSliceSpan {
  // NOLINTNEXTLINE(runtime/references)
  friend const MemSlice* begin(const MemSliceSpan &a);

  // NOLINTNEXTLINE(runtime/references)
  friend const MemSlice* end(const MemSliceSpan &a);

 public:
  enum { kMaxBlocks = 4 };  // only support 4 scattered blocks

  explicit MemSliceSpan(const MemBuf *buffer = nullptr);
  explicit MemSliceSpan(MemSlice buffer);
  explicit MemSliceSpan(std::initializer_list<MemSlice> blocks);

  MemSliceSpan(const void *buf, size_t length);

  MemSliceSpan(const MemSliceSpan &a);
  MemSliceSpan(MemSliceSpan &&rhs);

  ~MemSliceSpan();

  MemSliceSpan& operator=(const MemSliceSpan &a);
  MemSliceSpan& operator=(MemSliceSpan &&rhs);
  MemSliceSpan& operator=(const MemBuf *buffer);
  MemSliceSpan& operator=(MemSlice buffer);

  MemSliceSpan Clone() const;
  MemSliceSpan ToSingleBlock() const;

  // Create a copy of the spanned memory.
  MemBuf* CloneSpannedMem() const;

  // NOTE(liuyong): whether it is a single continuous memory block, otherwise,
  // it consists of couples of memory blocks.
  bool IsSingleBlock() const;

  bool Empty() const;
  uint32_t GetUsedSize() const;
  size_t GetBlockCount() const;

  base::RawBuffer ToRawBuffer() const;

  const MemSlice& operator[](size_t index) const;

  const MemSlice& GetFirstBlock() const;

  // NOTE(liuyong): Only pushing operation is supported
  bool PushBack(MemSlice block);
  void Reset(MemSlice block = MemSlice());

  void AdjustBeginPointer(uint32_t offset);

 private:
  uint32_t size_;
  int16_t block_count_;
  MemSlice buffer_[kMaxBlocks];
};

inline MemSliceSpan::MemSliceSpan(const MemSliceSpan &a) {
  size_ = a.size_;
  block_count_ = a.block_count_;

  for (int i = 0; i < block_count_; ++i) {
    buffer_[i] = a.buffer_[i];
  }
}

inline MemSliceSpan::MemSliceSpan(MemSliceSpan &&a) {
  size_ = a.size_;
  block_count_ = a.block_count_;

  for (int i = 0; i < block_count_; ++i) {
    buffer_[i] = std::move(a.buffer_[i]);
  }

  a.size_ = 0;
  a.block_count_ = 0;
}

inline MemSliceSpan::~MemSliceSpan() {
}

inline bool MemSliceSpan::IsSingleBlock() const {
  return block_count_ == 1;
}

inline const MemSlice& MemSliceSpan::GetFirstBlock() const {
  if (block_count_ == 0) {
    assert(false);
  }

  return buffer_[0];
}

// NOLINTNEXTLINE(runtime/references)
inline const MemSlice* begin(const MemSliceSpan &a) {
  return a.buffer_;
}

// NOLINTNEXTLINE(runtime/references)
inline const MemSlice* end(const MemSliceSpan &a) {
  return &a.buffer_[a.block_count_];
}

inline bool MemSliceSpan::Empty() const {
  return size_ == 0;
}

inline uint32_t MemSliceSpan::GetUsedSize() const {
  return size_;
}

inline size_t MemSliceSpan::GetBlockCount() const {
  return block_count_;
}

inline const MemSlice& MemSliceSpan::operator[](size_t i) const {
  assert(static_cast<int>(i) < block_count_);
  return buffer_[i];
}

}  // namespace memory
}  // namespace agora
