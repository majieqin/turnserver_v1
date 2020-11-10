// Copyright (c) 2014-2018 Agora.io, Inc.
// A safe and ref-counted buffer helper class to manage MemBuf
//

#pragma once  // NOLINT(build/header_guard)

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <utility>

#include "mem_buf.h"

namespace agora {
namespace memory {

// A ref-counted shared memory buffer
class MemSlice {
 public:
  explicit MemSlice(
      const MemBuf *buffer = nullptr, uint32_t size = 65536);

  MemSlice(const MemSlice &);
  MemSlice(MemSlice &&);

  ~MemSlice();

  MemSlice& operator=(const MemSlice &);
  MemSlice& operator=(MemSlice &&);

  MemSlice& operator=(const MemBuf *buf);

  bool UpdateBegin(uint32_t off_set);

  // Borrowed from |MemSlice|
  const MemBuf* GetMemBuf() const;

  // NOTE(liuyong): For compatilities, DO NOT use it again
  const MemBuf* GetUdpBuffer() const;

  // NOTE(liuyong): Sets the absolute position
  const void* SetBeginPointer(uint32_t pos);
  const void* SetEndPointer(uint32_t pos);

  void Reset(const MemBuf *buffer = nullptr);

  // NOTE(liuyong): Adjusts the pointer based on the current position
  const void* AdjustBeginPointer(int32_t offset);
  const void* AdjustEndPointer(int32_t offset);

  const void* Begin() const;
  const void* End() const;
  uint32_t GetUsedSize() const;

  // Clone a complete underlying |MemBuf|, and set the begin and end positions.
  MemSlice Clone() const;

  // Create a copy of the sliced memory.
  MemBuf* CloneSlicedMem() const;

 private:
  void Initialize(const MemBuf *buffer);

 private:
  const MemBuf *buffer_;

  uint32_t begin_offset_;
  uint32_t end_offset_;
};

inline MemSlice::MemSlice(const MemBuf *buffer, uint32_t size) {
  Initialize(buffer);

  if (buffer) {
    end_offset_ = std::min(size, buffer->Capacity());
  }
}

inline MemSlice::MemSlice(const MemSlice &rhs) {
  Initialize(rhs.buffer_);

  begin_offset_ = rhs.begin_offset_;
  end_offset_ = rhs.end_offset_;
}

inline MemSlice::MemSlice(MemSlice &&rhs) {
  Initialize(rhs.buffer_);

  if (rhs.buffer_) {
    rhs.buffer_->Release();
  }

  begin_offset_ = rhs.begin_offset_;
  end_offset_ = rhs.end_offset_;

  rhs.buffer_ = nullptr;
  rhs.begin_offset_ = 0;
  rhs.end_offset_ = 0;
}

inline void MemSlice::Initialize(const MemBuf *buffer) {
  begin_offset_ = 0;
  buffer_ = buffer;

  if (buffer_) {
    buffer_->AddRef();
    end_offset_ = buffer_->Capacity();
  } else {
    end_offset_ = 0;
  }
}

inline MemSlice::~MemSlice() {
  if (buffer_) {
    buffer_->Release();
  }
}

inline MemSlice& MemSlice::operator=(const MemSlice &rhs) {
  if (this == &rhs) {
    return *this;
  }

  if (buffer_) {
    buffer_->Release();
  }

  buffer_ = rhs.buffer_;
  if (buffer_) {
    buffer_->AddRef();
  }

  begin_offset_ = rhs.begin_offset_;
  end_offset_ = rhs.end_offset_;

  return *this;
}

inline MemSlice& MemSlice::operator=(MemSlice &&rhs) {
  std::swap(buffer_, rhs.buffer_);
  std::swap(begin_offset_, rhs.begin_offset_);
  std::swap(end_offset_, rhs.end_offset_);

  return *this;
}

inline bool MemSlice::UpdateBegin(uint32_t off_set) {
  if (begin_offset_ + off_set > end_offset_) {
    return false;
  }
  begin_offset_ += off_set;
  return true;
}

inline const MemBuf* MemSlice::GetMemBuf() const {
  return buffer_;
}

inline const MemBuf* MemSlice::GetUdpBuffer() const {
  return buffer_;
}

inline const void* MemSlice::Begin() const {
  return reinterpret_cast<const char *>(buffer_->Begin()) + begin_offset_;
}

inline const void* MemSlice::End() const {
  return reinterpret_cast<const char *>(buffer_->Begin()) + end_offset_;
}

inline uint32_t MemSlice::GetUsedSize() const {
  return static_cast<uint32_t>(end_offset_ - begin_offset_);
}

}  // namespace memory
}  // namespace agora
