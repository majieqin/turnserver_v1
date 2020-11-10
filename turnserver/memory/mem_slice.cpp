// Copyright (c) 2014-2018 Agora.io, Inc.
// Implementation of |MemSlice|

#include "media_server_library/memory/mem_slice.h"

#include <algorithm>
#include <utility>

#include "media_server_library/memory/mem_buf.h"

namespace agora {
namespace memory {

MemSlice& MemSlice::operator=(const MemBuf *buf) {
  if (buffer_) {
    buffer_->Release();
  }

  Initialize(buf);
  return *this;
}

const void* MemSlice::SetBeginPointer(uint32_t position) {
  if (position > end_offset_) {
    position = end_offset_;
  }

  begin_offset_ = position;
  return Begin();
}

const void* MemSlice::SetEndPointer(uint32_t position) {
  if (position < begin_offset_) {
    position = begin_offset_;
  }

  if (position > buffer_->Capacity()) {
    position = buffer_->Capacity();
  }

  end_offset_ = position;
  return End();
}

const void* MemSlice::AdjustBeginPointer(int32_t offset) {
  auto position =
      static_cast<uint32_t>(static_cast<int32_t>(begin_offset_) + offset);

  return SetBeginPointer(position);
}

const void* MemSlice::AdjustEndPointer(int32_t offset) {
  auto position =
      static_cast<uint32_t>(static_cast<int32_t>(end_offset_) + offset);

  return SetEndPointer(position);
}

void MemSlice::Reset(const MemBuf *buf) {
  if (buffer_) {
    buffer_->Release();
  }

  Initialize(buf);
}

MemSlice MemSlice::Clone() const {
  MemSlice tmp(buffer_->Clone());

  tmp.begin_offset_ = begin_offset_;
  tmp.end_offset_ = end_offset_;

  return tmp;
}

MemBuf* MemSlice::CloneSlicedMem() const {
  return MemBuf::Create(Begin(), GetUsedSize());
}

}  // namespace memory
}  // namespace agora
