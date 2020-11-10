// Copyright (c) 2014-2017 Agora.io, Inc.
// Base class for packing & unpacking protocols xmitted through networks
//

#include "unpacker.h"

#include <cassert>
#include <cstring>
#include <stdexcept>

namespace agora {
namespace base {

Unpacker::Unpacker(const void *buf, uint32_t len, bool copy)
    : copy_(copy),
      length_(len),
      buffer_(reinterpret_cast<const uint8_t *>(buf)),
      position_(0) {
  Initialize();
}

Unpacker::Unpacker(const char *buf, uint32_t len, bool copy)
    : copy_(copy), length_(len), position_(0) {
  buffer_ = reinterpret_cast<const uint8_t *>(buf);
  Initialize();
}

void Unpacker::Initialize() {
  if (copy_) {
    auto temp = new (std::nothrow) uint8_t[length_];
    ::memcpy(temp, buffer_, length_);
    buffer_ = temp;
  }

  uint32_t length = PopVariableUint32();
  (void)length;

  // assert(length == len);
}

Unpacker::~Unpacker() {
  if (copy_) {
    delete []buffer_;
  }
}

}  // namespace base
}  // namespace agora
