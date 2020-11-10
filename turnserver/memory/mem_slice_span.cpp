// Copyright (c) 2014-2018 Agora.io, Inc.
// Fast MemBuf for accerleration
//

#include "media_server_library/memory/mem_slice_span.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <functional>

namespace agora {
namespace memory {

MemSliceSpan::MemSliceSpan(const MemBuf *buffer) {
  if (!buffer) {
    size_ = 0;
    block_count_ = 0;
    return;
  }

  block_count_ = 1;
  size_ = buffer->Capacity();

  buffer_[0] = MemSlice(buffer);
}

MemSliceSpan::MemSliceSpan(MemSlice buffer) {
  if (buffer.GetUsedSize() == 0) {
    size_ = 0;
    block_count_ = 0;
    return;
  }

  block_count_ = 1;
  size_ = buffer.GetUsedSize();
  buffer_[0] = std::move(buffer);
}

MemSliceSpan::MemSliceSpan(std::initializer_list<MemSlice> blocks) {
  assert(blocks.size() < kMaxBlocks);

  size_ = 0;
  block_count_ = 0;

  for (auto it = blocks.begin();
       it != blocks.end() && block_count_ < kMaxBlocks; ++it) {
    size_ += it->GetUsedSize();
    buffer_[block_count_++] = std::move(*it);
  }
}

MemSliceSpan::MemSliceSpan(const void *buf, size_t length) {
  size_ = length;
  block_count_ = 1;

  buffer_[0] = MemSlice(MemBuf::Create(buf, length));
}

MemSliceSpan& MemSliceSpan::operator=(const MemSliceSpan &a) {
  if (this == &a) {
    return *this;
  }

  assert(a.block_count_ <= kMaxBlocks);

  for (int i = 0; i < a.block_count_; ++i) {
    buffer_[i] = a.buffer_[i];
  }

  for (int i = a.block_count_; i < block_count_; ++i) {
    buffer_[i].Reset();
  }

  size_ = a.size_;
  block_count_ = a.block_count_;

  return *this;
}

MemSliceSpan& MemSliceSpan::operator=(MemSliceSpan &&a) {
  if (this == &a) {
    return *this;
  }

  assert(a.block_count_ <= kMaxBlocks);

  for (int i = 0; i < a.block_count_; ++i) {
    buffer_[i] = std::move(a.buffer_[i]);
  }

  for (int i = a.block_count_; i < block_count_; ++i) {
    buffer_[i].Reset();
  }

  size_ = a.size_;
  block_count_ = a.block_count_;

  a.size_ = 0;
  a.block_count_ = 0;
  return *this;
}

MemSliceSpan& MemSliceSpan::operator=(MemSlice buffer) {
  for (int i = 1; i < block_count_; ++i) {
    buffer_[i].Reset();
  }

  size_ = buffer.GetUsedSize();
  buffer_[0] = std::move(buffer);
  block_count_ = 1;
  return *this;
}

MemSliceSpan& MemSliceSpan::operator=(const MemBuf *buf) {
  return operator=(MemSlice(buf));
}

bool MemSliceSpan::PushBack(MemSlice block) {
  if (block_count_ >= kMaxBlocks) {
    return false;
  }

  size_ += block.GetUsedSize();
  buffer_[block_count_++] = std::move(block);
  return true;
}

void MemSliceSpan::Reset(MemSlice block) {
  for (int i = block_count_ - 1; i >= 0; --i) {
    buffer_[i].Reset();
  }

  buffer_[0] = std::move(block);

  size_ = buffer_[0].GetUsedSize();
  block_count_ = (size_ == 0) ? 0 : 1;
}

MemSliceSpan MemSliceSpan::Clone() const {
  assert(block_count_ < kMaxBlocks);

  MemSliceSpan tmp;

  for (auto i = 0; i < block_count_; ++i) {
    tmp.buffer_[i] = buffer_[i].Clone();
  }

  tmp.size_ = size_;
  tmp.block_count_ = block_count_;
  return tmp;
}

MemSliceSpan MemSliceSpan::ToSingleBlock() const {
  assert(block_count_ < kMaxBlocks);

  if (block_count_ <= 1) {
    return *this;
  }

  return MemSliceSpan(CloneSpannedMem());
}

MemBuf *MemSliceSpan::CloneSpannedMem() const {
  assert(block_count_ < kMaxBlocks);

  auto p = MemBuf::Create(size_);
  auto q = reinterpret_cast<uint8_t *>(p->Begin());

  for (auto i = 0; i < block_count_; ++i) {
    ::memcpy(q, buffer_[i].Begin(), buffer_[i].GetUsedSize());
    q += buffer_[i].GetUsedSize();
  }

  return p;
}

base::RawBuffer MemSliceSpan::ToRawBuffer() const {
  assert(block_count_ < kMaxBlocks);
  if (block_count_ <= 0 || size_ == 0) {
    return base::RawBuffer();
  }

  base::RawBuffer buffer;
  buffer.reserve(size_);

  for (auto i = 0; i < block_count_; ++i) {
    auto start = reinterpret_cast<const uint8_t *>(buffer_[i].Begin());
    auto end = reinterpret_cast<const uint8_t *>(buffer_[i].End());
    buffer.insert(buffer.end(), start, end);
  }

  return buffer;
}

void MemSliceSpan::AdjustBeginPointer(uint32_t offset) {
  assert(offset < size_);
  if (offset >= size_) {
    Reset();
    return;
  }

  auto left = offset;
  int32_t pos = 0;
  for (; pos < block_count_; ++pos) {
    auto &block = buffer_[pos];
    auto count = std::min<uint32_t>(left, block.GetUsedSize());
    block.AdjustBeginPointer(count);
    left -= count;
    if (left == 0) {
      if (block.GetUsedSize() == 0) {
        ++pos;
      }
      break;
    }
  }

  auto block_count = block_count_;
  block_count_ -= pos;
  size_ -= offset;

  if (pos == 0) {
    return;
  }

  for (int32_t k = 0; pos < block_count; ++pos) {
    buffer_[k] = std::move(buffer_[pos]);
  }
}

}  // namespace memory
}  // namespace agora
