// Copyright (c) 2014-2017 Agora.io, Inc.
// Base class for packing & unpacking protocols xmitted through networks
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <cstring>
#include <map>
#include <set>
#include <list>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <iostream>

namespace agora {
namespace base {

class Unpacker {
 public:
  Unpacker(const char *buf, uint32_t len, bool copy = false);
  Unpacker(const void *buf, uint32_t len, bool copy = false);

  ~Unpacker();

  Unpacker& Rewind();
  void Reset();

  uint32_t PopVariableUint32();

  bool PopBoolean();
  uint8_t PopUint8();
  uint16_t PopUint16();
  uint32_t PopUint32();
  uint64_t PopUint64();
  double PopDouble();
  std::string PopString();
  std::vector<uint8_t> PopBuffer();
  void PopBinary(void *buf, size_t length);

  const uint8_t* Buffer() const;
  uint32_t Length() const;

 private:
  void Initialize();
  void CheckSize(uint32_t more, uint32_t position) const;

  const bool copy_;
  const uint32_t length_;

  const uint8_t *buffer_;
  uint32_t position_;
};

inline Unpacker& Unpacker::Rewind() {
  position_ = 0;
  PopVariableUint32();

  return *this;
}

inline void Unpacker::Reset() {
  position_ = 0;
}

inline uint32_t Unpacker::PopUint32() {
  uint32_t v = 0;
  CheckSize(sizeof(v), position_);

  ::memcpy(&v, buffer_ + position_, sizeof(v));
  position_ += sizeof(v);
  return v;
}

inline uint32_t Unpacker::PopVariableUint32() {
  uint32_t v = 0;
  CheckSize(2, position_);

  const uint16_t *pos = reinterpret_cast<const uint16_t *>(buffer_ + position_);
  if (*pos < 0x8000) {
    v = *pos;
    position_ += 2;
  } else {
    CheckSize(3, position_);

    uint32_t value = *reinterpret_cast<const uint8_t *>(pos + 1);
    v = (value << 15) + ((*pos) & 0x7FFF);
    position_ += 3;
  }

  return v;
}

inline uint16_t Unpacker::PopUint16() {
  uint16_t v = 0;
  CheckSize(sizeof(v), position_);
  ::memcpy(&v, buffer_ + position_, sizeof(v));
  position_ += sizeof(v);
  return v;
}

inline bool Unpacker::PopBoolean() {
  bool v = false;
  CheckSize(sizeof(v), position_);
  ::memcpy(&v, buffer_ + position_, sizeof(v));
  position_ += sizeof(v);
  return v;
}

inline uint8_t Unpacker::PopUint8() {
  uint8_t v = 0;
  CheckSize(sizeof(v), position_);
  ::memcpy(&v, buffer_ + position_, sizeof(v));
  position_ += sizeof(v);
  return v;
}

inline uint64_t Unpacker::PopUint64() {
  uint64_t v = 0;
  CheckSize(sizeof(v), position_);

  ::memcpy(&v, buffer_ + position_, sizeof(v));
  position_ += sizeof(v);
  return v;
}

inline double Unpacker::PopDouble() {
  double v = 0;
  CheckSize(sizeof(v), position_);

  ::memcpy(&v, buffer_ + position_, sizeof(v));
  position_ += sizeof(v);
  return v;
}

inline std::string Unpacker::PopString() {
  uint32_t length = PopVariableUint32();
  CheckSize(length, position_);

  auto p = reinterpret_cast<const char *>(buffer_ + position_);
  std::string s = std::string(p, length);
  position_ += length;
  return s;
}

inline std::vector<uint8_t> Unpacker::PopBuffer() {
  uint32_t length = PopVariableUint32();
  CheckSize(length, position_);

  auto p = reinterpret_cast<const uint8_t *>(buffer_) + position_;
  auto s = std::vector<uint8_t>(p, p + length);
  position_ += length;
  return s;
}

inline void Unpacker::PopBinary(void *buf, size_t length) {
  CheckSize(length, position_);

  auto p = reinterpret_cast<const uint8_t *>(buffer_) + position_;
  ::memcpy(buf, p, length);
  position_ += length;
}

inline const uint8_t* Unpacker::Buffer() const {
  return buffer_;
}

inline uint32_t Unpacker::Length() const {
  return length_;
}

inline void Unpacker::CheckSize(uint32_t more, uint32_t position) const {
  if (length_ < position + more) {
    //throw std::underflow_error("Unpacker buffer underflow!");
    std::cerr << "Unpacker buffer overflow!" << std::endl;
  }
}

inline Unpacker& operator>>(Unpacker &p,  // NOLINT(runtime/references)
                            char &v) {  // NOLINT(runtime/references)
  v = static_cast<char>(p.PopUint8());
  return p;
}

inline Unpacker& operator>>(Unpacker &p,  // NOLINT(runtime/references)
                            bool &v) {  // NOLINT(runtime/references)
  v = p.PopBoolean();
  return p;
}

inline Unpacker& operator>>(Unpacker &p,  // NOLINT(runtime/references)
                            uint8_t &v) {  // NOLINT(runtime/references)
  v = p.PopUint8();
  return p;
}

inline Unpacker& operator>>(Unpacker &p,  // NOLINT(runtime/references)
                            uint16_t &v) {  // NOLINT(runtime/references)
  v = p.PopUint16();
  return p;
}

inline Unpacker& operator>>(Unpacker &p,  // NOLINT(runtime/references)
                            uint32_t &v) {  // NOLINT(runtime/references)
  v = p.PopUint32();
  return p;
}

inline Unpacker& operator>>(Unpacker &p,  // NOLINT(runtime/references)
                            uint64_t &v) {  // NOLINT(runtime/references)
  v = p.PopUint64();
  return p;
}

inline Unpacker& operator>>(Unpacker &p,  // NOLINT(runtime/references)
                            int8_t &v) {  // NOLINT(runtime/references)
  v = static_cast<int8_t>(p.PopUint8());
  return p;
}

inline Unpacker& operator>>(Unpacker &p,  // NOLINT(runtime/references)
                            int16_t &v) {  // NOLINT(runtime/references)
  v = static_cast<int16_t>(p.PopUint16());
  return p;
}

inline Unpacker& operator>>(Unpacker &p,  // NOLINT(runtime/references)
                            int32_t &v) {  // NOLINT(runtime/references)
  v = static_cast<int32_t>(p.PopUint32());
  return p;
}

inline Unpacker& operator>>(Unpacker &p,  // NOLINT(runtime/references)
                            int64_t &v) {  // NOLINT(runtime/references)
  v = static_cast<int64_t>(p.PopUint64());
  return p;
}

inline Unpacker& operator>>(Unpacker &p,  // NOLINT(runtime/references)
                            double &v) {  // NOLINT(runtime/references)
  v = static_cast<double>(p.PopDouble());
  return p;
}

inline Unpacker& operator>>(Unpacker &p,  // NOLINT(runtime/references)
                            std::string &s) {  // NOLINT(runtime/references)
  s = p.PopString();
  return p;
}

inline Unpacker& operator>>(
    Unpacker &p,  // NOLINT(runtime/references)
    std::vector<uint8_t> &s) {  // NOLINT(runtime/references)
  s = p.PopBuffer();
  return p;
}

template <typename T>
inline Unpacker& operator>>(
    Unpacker &p,  // NOLINT(runtime/references)
    std::vector<T> &v) {  // NOLINT(runtime/references)
  uint32_t count = p.PopVariableUint32();
  for (uint32_t i = 0; i < count; ++i) {
    T t;
    p >> t;
    v.push_back(std::move(t));
  }

  return p;
}

template <typename T>
inline Unpacker& operator>>(
    Unpacker &p,  // NOLINT(runtime/references)
    std::list<T> &v) {  // NOLINT(runtime/references)
  uint32_t count = p.PopVariableUint32();
  for (uint32_t i = 0; i < count; ++i) {
    T t;
    p >> t;
    v.push_back(std::move(t));
  }

  return p;
}

template <typename K, typename V>
inline Unpacker& operator>>(Unpacker &p,  // NOLINT(runtime/references)
                            std::pair<K, V> &v) {  // NOLINT(runtime/references)
  p >> v.first >> v.second;
  return p;
}

template <typename T> inline Unpacker& operator>>(
    Unpacker &p, std::set<T> &v) {  // NOLINT(runtime/references)
  uint32_t count = p.PopVariableUint32();
  for (uint32_t i = 0; i < count; ++i) {
    T t;
    p >> t;
    v.insert(std::move(t));
  }

  return p;
}

template <typename K, typename V>
inline Unpacker& operator>>(
    Unpacker &p, std::map<K, V> &x) {  // NOLINT(runtime/references)
  uint32_t count = p.PopVariableUint32();
  for (uint32_t i = 0; i < count; ++i) {
    K k;
    V v;
    p >> k >> v;
    x.insert(std::make_pair(std::move(k), std::move(v)));
  }

  return p;
}

template<typename K, typename V>
inline Unpacker& operator>>(
    Unpacker &p, std::unordered_map<K, V> &x) {  // NOLINT(runtime/references)
  uint32_t count = p.PopVariableUint32();
  for (uint32_t i = 0; i < count; ++i) {
    K k;
    V v;
    p >> k >> v;
    x.insert(std::make_pair(std::move(k), std::move(v)));
  }

  return p;
}

}  // namespace base
}  // namespace agora
