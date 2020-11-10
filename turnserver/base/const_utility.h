// Copyright (c) 2014-2017 Agora.io, Inc.
// Utilities for constexpr
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <iostream>


namespace agora {
namespace base {

constexpr bool StringEqual(const char* a, const char* b, std::size_t n) {
  return (*a == 0 && *b == 0 && n == 0) ? true :
         (*a == 0 || *b == 0) ? false :
         n == 0 ? true :
         *a != *b ? false :
         StringEqual(a+1, b+1, n-1);
}

struct OutOfRange {
  OutOfRange(uint32_t, uint32_t) {}
};

constexpr bool RequiresInRange(uint32_t i, uint32_t len) {
  //return i >= len ? throw OutOfRange(i, len) : i;
  return i >= len ? len : i;
}

class StringRef {
 public:
  template<uint32_t N>
  constexpr StringRef(const char (&arr)[N]) : begin_(arr), size_(N-1) {
    static_assert(N >= 1, "const string literal should not be empty");
  }

  constexpr StringRef(const char* buf, uint32_t len)
    : begin_(buf), size_(len) {}

  constexpr char operator[](uint32_t i) const {
    return begin_[RequiresInRange(i, size_)];
  }

  constexpr const char* Begin() const {
    return begin_;
  }

  constexpr const char* End() const {
    return begin_ + size_;
  }

  constexpr uint32_t Size() const {
    return size_;
  }

  constexpr StringRef SubString(int pos, int len) const {
    return RequiresInRange(pos, size_), RequiresInRange(pos + len, size_),
           StringRef(begin_ + pos, len);
  }

 private:
  const char* const begin_;
  uint32_t size_;
};

constexpr bool operator==(StringRef a, StringRef b) {
  return a.Size() != b.Size() ? false
       : StringEqual(a.Begin(), b.Begin(), a.Size());
}

constexpr StringRef operator "" _ref (const char* str, std::size_t N) {
  return StringRef(str, N);
}
}  // namespace base
}  // namespace agora
