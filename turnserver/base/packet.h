// Copyright (c) 2014-2017 Agora.io, Inc.
// Base class for packing & unpacking protocols xmitted through networks
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <vector>

#include "packer.h"
#include "unpacker.h"

namespace agora {
namespace base {

class Packet {
 public:
  explicit Packet(uint16_t u);
  explicit Packet(uint16_t server, uint16_t u);

  Packet(const Packet &) = default;
  Packet(Packet &&) = default;

  Packet& operator =(const Packet &) = default;
  Packet& operator =(Packet &&) = default;

  virtual ~Packet();

  void SetUri(uint16_t uri);

  uint16_t server_type() const;
  uint16_t uri() const;

  virtual void Marshall(Packer *p) const;
  virtual void Unmarshall(Unpacker *p);
  virtual void Pack(Packer *p) const;

 private:
  uint16_t server_type_;
  uint16_t uri_;
};

inline Packet::Packet(uint16_t u)
    : server_type_(0), uri_(u) {
}

inline Packet::Packet(uint16_t server_type, uint16_t u)
    : server_type_(server_type), uri_(u) {
}

inline Packet::~Packet() {
}

inline void Packet::SetUri(uint16_t uri) {
  uri_ = uri;
}

inline uint16_t Packet::server_type() const {
  return server_type_;
}

inline uint16_t Packet::uri() const {
  return uri_;
}

inline void Packet::Marshall(Packer *p) const {
  *p << server_type_ << uri_;
}

inline void Packet::Unmarshall(Unpacker *p) {
  *p >> server_type_ >> uri_;
}

inline void Packet::Pack(Packer *p) const {
  Marshall(p);
  p->Pack();
}

inline Packer& operator<<(Packer &p,  // NOLINT(runtime/references)
                          const Packet &x) {
  x.Marshall(&p);
  return p;
}

inline Unpacker& operator>>(Unpacker &p,  // NOLINT(runtime/references)
                            Packet &x) {  // NOLINT(runtime/references)
  x.Unmarshall(&p);
  return p;
}

template <typename T>
inline std::vector<uint8_t> PackedBuffer(const T &t) {
  Packer pkr;
  pkr << t;
  pkr.Pack();
  return pkr.Body2();
}

}  // namespace base
}  // namespace agora
