// Copyright (c) 2014-2017 Agora.io, Inc.
// Base class for packing & unpacking protocols xmitted through networks
//

#pragma once  // NOLINT(build/header_guard)

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <iostream>

#include "declare_struct.h"

#include "../memory/mem_buf.h"
#include "../memory/mem_slice.h"

namespace agora {
namespace base {

enum class PacketLimit : uint32_t {
  kMaxPacketSize = 8 * 1024 * 1024 - 1024,
};

class Packer : private base::NonCopyOrMovable {
 public:
  enum {
    kDefaultSize = 1024,
  };

  static uint16_t VariableUint32Length(uint32_t length);

  explicit Packer(uint32_t buffer_sz = kDefaultSize);

  // After |Pack| is called. All |Pushxxx| functions should not be called
  // until |Reset| is called.
  Packer& Pack();
  void Reset();

  void WriteUint16(uint16_t val, uint32_t position);
  void WriteUint32(uint32_t val, uint32_t position);

  void PushBoolean(bool val);
  void PushUint8(uint8_t val);
  void PushUint16(uint16_t val);
  void PushUint32(uint32_t val);
  void PushUint64(uint64_t val);
  void PushDouble(double val);

  // This |value| should not larger than 8 * 1024 * 1024
  // Return value:
  // 2, |value| is less than 32 * 1024, otherwise, 3 returned
  int PushVariableUint32(uint32_t value);

  void PushString(const std::string &val);
  void PushBuffer(const std::vector<uint8_t> &val);
  void PushBinary(const void *data, size_t length);

  const char *Buffer() const;
  uint32_t Length() const;

  // The following two functions are valid only after |Pack| is called.
  std::string Data() const;
  std::string Body() const;

  std::vector<uint8_t> Data2() const;
  std::vector<uint8_t> Body2() const;

  const uint8_t *BodyStart() const;
  uint32_t BodySize() const;

  memory::MemSlice TakeBuffer();

 private:
  struct MemBufDeleter {
    void operator()(memory::MemBuf *buf) const {
      buf->Release();
    }
  };

  typedef std::unique_ptr<memory::MemBuf, MemBufDeleter> MemBufPtr;

  void CheckSize(uint32_t more, uint32_t position);

  char* Begin();
  const char* Begin() const;
  char* End();

 private:
  MemBufPtr mem_;
  uint32_t length_;
  uint32_t position_;
};

inline Packer::Packer(uint32_t buffer_sz) {
  mem_.reset(memory::MemBuf::Create(std::max<uint32_t>(64u, buffer_sz)));

  length_ = 0;
  position_ = 4;
}

inline Packer& Packer::Pack() {
  assert(position_ < static_cast<uint32_t>(PacketLimit::kMaxPacketSize));

  auto start = 0;
  if (position_ >= 0x8002) {
    length_ = position_ - 1;  // 3-bytes length
    position_ = 1;
    start = 1;
  } else {
    length_ = position_ - 2;
    position_ = 2;
    start = 2;
  }

  int size = PushVariableUint32(length_);
  (void)size;

  position_ = start;
  return *this;
}

inline void Packer::Reset() {
  length_ = 0;
  position_ = 4;
}

inline void Packer::WriteUint16(uint16_t val, uint32_t position) {
  CheckSize(sizeof(val), position);
  ::memcpy(Begin() + position, &val, sizeof(val));
}

inline void Packer::WriteUint32(uint32_t val, uint32_t position) {
  CheckSize(sizeof(val), position);
  ::memcpy(Begin() + position, &val, sizeof(val));
}

inline void Packer::PushBoolean(bool val) {
  char cval = static_cast<char>(val);
  PushUint8(cval);
}

inline void Packer::PushUint8(uint8_t val) {
  CheckSize(sizeof(val), position_);

  ::memcpy(Begin() + position_, &val, sizeof(val));
  position_ += sizeof(val);
}

inline void Packer::PushUint16(uint16_t val) {
  CheckSize(sizeof(val), position_);

  ::memcpy(Begin() + position_, &val, sizeof(val));
  position_ += sizeof(val);
}

inline void Packer::PushUint32(uint32_t val) {
  CheckSize(sizeof(val), position_);

  ::memcpy(Begin() + position_, &val, sizeof(val));
  position_ += sizeof(val);
}

inline void Packer::PushUint64(uint64_t val) {
  CheckSize(sizeof(val), position_);

  ::memcpy(End(), &val, sizeof(val));
  position_ += sizeof(val);
}

inline void Packer::PushDouble(double val) {
  CheckSize(sizeof(val), position_);

  ::memcpy(End(), &val, sizeof(val));
  position_ += sizeof(val);
}

inline void Packer::PushString(const std::string &val) {
  uint32_t length = static_cast<uint32_t>(val.length());
  PushVariableUint32(length);

  CheckSize(length, position_);

  if (length > 0) {
    ::memcpy(End(), val.data(), length);
    position_ += length;
  }
}

inline void Packer::PushBuffer(const std::vector<uint8_t> &val) {
  uint32_t length = static_cast<uint32_t>(val.size());
  PushVariableUint32(length);

  CheckSize(length, position_);

  if (length > 0) {
    ::memcpy(End(), val.data(), length);
    position_ += length;
  }
}

inline void Packer::PushBinary(const void *data, size_t length) {
  CheckSize(length, position_);

  if (length > 0) {
    ::memcpy(End(), data, length);
    position_ += static_cast<uint32_t>(length);
  }
}

inline uint16_t Packer::VariableUint32Length(uint32_t value) {
  if (value < 0x8000) {
    return 2;
  } else {
    return 3;
  }

  // NOTE(liuyong): Never run through here
  return -1;
}

inline int Packer::PushVariableUint32(uint32_t value) {
  CheckSize(3, position_);

  auto pos = reinterpret_cast<uint16_t *>(End());
  if (value < 0x8000) {
    *pos = value;
    position_ += 2;
    return 2;
  } else {
    uint16_t lower = static_cast<uint16_t>((value & 0x7FFF) | 0x8000);
    *pos++ = lower;

    uint8_t upper = static_cast<uint8_t>(value >> 15);
    uint8_t *p = reinterpret_cast<uint8_t *>(pos);
    *p = upper;

    position_ += 3;
    return 3;
  }

  // NOTE(liuyong): Never run through here
  return -1;
}

inline const char* Packer::Buffer() const {
  return Begin() + position_;
}

inline uint32_t Packer::Length() const { return length_; }

inline std::string Packer::Data() const {
  return std::string(Begin() + position_, length_);
}

inline std::string Packer::Body() const {
  if (length_ >= 0x8000) {
    return std::string(Begin() + 4, length_ - 3);
  } else {
    return std::string(Begin() + 4, length_ - 2);
  }
}

inline std::vector<uint8_t> Packer::Data2() const {
  auto *p = reinterpret_cast<const uint8_t *>(Begin() + position_);
  return std::vector<uint8_t>(p, p + length_);
}

inline const uint8_t *Packer::BodyStart() const {
  return reinterpret_cast<const uint8_t *>(Begin() + 4);
}

inline uint32_t Packer::BodySize() const {
  if (length_ >= 0x8000) {
    return length_ - 3;
  }

  return length_ - 2;
}

inline std::vector<uint8_t> Packer::Body2() const {
  auto p = reinterpret_cast<const uint8_t *>(Begin() + 4);

  if (length_ >= 0x8000) {
    return std::vector<uint8_t>(p, p + length_ - 3);
  }

  return std::vector<uint8_t>(p, p + length_ - 2);
}

inline void Packer::CheckSize(uint32_t more, uint32_t position) {
  uint32_t new_size = position + more;

  if (new_size >= static_cast<uint32_t>(PacketLimit::kMaxPacketSize)) {
    std::cerr << "packer buffer overflow!" << std::endl;
    // throw std::overflow_error("packer buffer overflow!");
  }

  auto old_cap = mem_->Capacity();
  if (old_cap < new_size) {
    auto size = std::max<uint32_t>(new_size, 2 * old_cap);
    mem_.reset(memory::MemBuf::Create(mem_->Begin(), old_cap, size));
  }
}

inline const char* Packer::Begin() const {
  return reinterpret_cast<const char *>(mem_->Begin());
}

inline char* Packer::Begin() {
  return reinterpret_cast<char *>(mem_->Begin());
}

inline char* Packer::End() {
  return Begin() + position_;
}

inline memory::MemSlice Packer::TakeBuffer() {
  memory::MemSlice slice(mem_.release());
  slice.SetBeginPointer(position_);
  slice.SetEndPointer(position_ + length_);
  return slice;
}

inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          bool v) {
  p.PushBoolean(v);
  return p;
}

inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          uint8_t v) {
  p.PushUint8(v);
  return p;
}

inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          uint16_t v) {
  p.PushUint16(v);
  return p;
}

inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          uint32_t v) {
  p.PushUint32(v);
  return p;
}

inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          uint64_t v) {
  p.PushUint64(v);
  return p;
}

inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          char v) {
  p.PushUint8(static_cast<uint8_t>(v));
  return p;
}

inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          int8_t v) {
  p.PushUint8(static_cast<uint8_t>(v));
  return p;
}

inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          int16_t v) {
  p.PushUint16(static_cast<uint16_t>(v));
  return p;
}

inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          int32_t v) {
  p.PushUint32(static_cast<uint32_t>(v));
  return p;
}

inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          int64_t v) {
  p.PushUint64(static_cast<uint64_t>(v));
  return p;
}

inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          double v) {
  p.PushDouble(static_cast<double>(v));
  return p;
}

inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          const std::vector<uint8_t> &v) {
  p.PushBuffer(v);
  return p;
}

inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          const std::string &v) {
  p.PushString(v);
  return p;
}

template <typename T>
inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          const std::vector<T> &v) {
  uint32_t count = static_cast<uint32_t>(v.size());
  p.PushVariableUint32(count);

  for (uint32_t i = 0; i < count; ++i) {
    p << v[i];
  }

  return p;
}

template <typename T>
inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          const std::list<T> &v) {
  uint32_t count = static_cast<uint32_t>(v.size());
  p.PushVariableUint32(count);

  for (const T &x : v) {
    p << x;
  }

  return p;
}

template <typename T>
inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          const std::set<T> &v) {
  uint32_t count = static_cast<uint32_t>(v.size());
  p.PushVariableUint32(count);

  for (const T &x : v) {
    p << x;
  }

  return p;
}

template <typename K, typename V>
inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          const std::pair<K, V> &value) {
  p << value.first << value.second;
  return p;
}

template <typename K, typename V>
inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          const std::map<K, V> &v) {
  uint32_t count = static_cast<uint32_t>(v.size());
  p.PushVariableUint32(count);

  for (const auto &x : v) {
    p << x;
  }

  return p;
}

template <typename K, typename V>
inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          const std::unordered_map<K, V> &v) {
  uint32_t count = static_cast<uint32_t>(v.size());
  p.PushVariableUint32(count);

  for (const auto &x : v) {
    p << x;
  }

  return p;
}

template <typename T, size_t Len>
inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          const std::array<T, Len> &arr) {
  for (const auto &x : arr) {
    p << x;
  }
  return p;
}

template <typename K>
inline Packer &operator<<(Packer &p,  // NOLINT(runtime/references)
                          const std::unordered_set<K> &s) {
  uint32_t count = static_cast<uint32_t>(s.size());
  p.PushVariableUint32(count);

  for (const auto &x : s) {
    p << x;
  }

  return p;
}

}  // namespace base
}  // namespace agora
