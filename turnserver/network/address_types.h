// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
// Network address types

#pragma once  // NOLINT(build/header_guard)

#include <netinet/in.h>
#include <sys/un.h>

#include <cassert>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../base/declare_struct.h"

namespace agora {
namespace network {

enum class IpVersion {
  kIp4 = 0,
  kIp6 = 1,
  kUnix = 2,
};

NEW_DECLARE_PACKABLE_1_START(SocketAddressUnix, std::string, path)
  explicit SocketAddressUnix(const sockaddr_un &address)
      : path(address.sun_path) {
  }

  SocketAddressUnix& operator=(const sockaddr_un &addr);

  std::string ToDebugString() const;
  operator sockaddr_un() const;

  // return path
  std::string Path() const;
NEW_DECLARE_STRUCT_END

inline bool operator>(const SocketAddressUnix &a,
                      const SocketAddressUnix &b) {
  return a.path > b.path;
}

// NOTE: ip/port should use network order
// NOTE(liuyong): It is deprecated
NEW_DECLARE_PACKABLE_2_START(SocketAddress4, uint32_t, ip, uint16_t, port)
  explicit SocketAddress4(const sockaddr_in &address)
      : ip(address.sin_addr.s_addr), port(address.sin_port) {
  }

  SocketAddress4& operator=(const sockaddr_in &addr);

  std::string ToDebugString() const;
  operator sockaddr_in() const;

  // return ip in host order
  uint32_t Ip() const;
  // return port in host order
  uint16_t Port() const;

NEW_DECLARE_STRUCT_END

inline uint32_t SocketAddress4::Ip() const {
  return ntohl(ip);
}
inline uint16_t SocketAddress4::Port() const {
  return ntohs(port);
}

inline bool operator>(const SocketAddress4 &a, const SocketAddress4 &b) {
  return a.ip > b.ip || (a.ip == b.ip && a.port > b.port);
}

struct SocketAddress4Hasher {
  size_t operator()(const SocketAddress4 &addr) const;

 private:
  std::hash<uint64_t> hasher_;
};

inline size_t SocketAddress4Hasher::operator()(const SocketAddress4 &a) const {
  union {
    uint64_t repr;
    struct {
      uint32_t ip;
      uint16_t port;
      uint16_t zero;
    } detail;
  } content;

  static_assert(sizeof(content) == sizeof(uint64_t), "Alignment violated");

  content.detail.ip = a.ip;
  content.detail.port = a.port;
  content.detail.zero = 0;

  return hasher_(content.repr);
}

NEW_DECLARE_PACKABLE_2(AddressPair4, SocketAddress4, first,
                       SocketAddress4, second)

typedef std::vector<AddressPair4> RouterAddressList;

// NOTE(liuyong): |SocketAddress4| is deprecated due to its inconsistent
// interface.
// NOTE: ip/port should use host-order
class NewSockAddress4 {
 public:
  explicit NewSockAddress4(uint32_t ip = INADDR_ANY, uint16_t port = 0);

  // NOLINTNEXTLINE(runtime/explicit)
  NewSockAddress4(const SocketAddress4 &address);

  // NOLINTNEXTLINE(runtime/explicit)
  NewSockAddress4(const sockaddr_in &address);

  NewSockAddress4& operator=(const SocketAddress4 &addr);
  NewSockAddress4& operator=(const sockaddr_in &addr);

  std::string ToDebugString() const;
  operator sockaddr_in() const;

  // Returns ip in host order
  uint32_t ip() const;

  // Returns port in host order
  uint16_t port() const;

  // host-order
  void SetIp(uint32_t ip);
  void SetPort(uint16_t port);

  // Serialization
  void SerializeTo(base::Packer *p) const;
  void SerializeFrom(base::Unpacker *p);

 private:
  uint32_t ip_;
  uint16_t port_;
};

bool operator<(const NewSockAddress4 &a, const NewSockAddress4 &b);
bool operator==(const NewSockAddress4 &a, const NewSockAddress4 &b);
bool operator!=(const NewSockAddress4 &a, const NewSockAddress4 &b);

base::Packer& operator<<(base::Packer &pkr, const NewSockAddress4 &addr);
base::Unpacker& operator>>(base::Unpacker &pkr, NewSockAddress4 &addr);

#include "address_types.inl"

NEW_DECLARE_PACKABLE_2(Address4Pair, NewSockAddress4, first,
                       NewSockAddress4, second)

typedef std::vector<Address4Pair> RouterAddressList2;

struct NewSockAddress4Hasher {
  size_t operator()(const NewSockAddress4 &addr) const;

 private:
  std::hash<uint64_t> hasher_;
};

inline size_t NewSockAddress4Hasher::operator()(
    const NewSockAddress4 &a) const {
  union {
    uint64_t repr;
    struct {
      uint32_t ip;
      uint32_t port;
    } detail;
  } content;

  static_assert(sizeof(content) == sizeof(uint64_t), "Alignment violated");

  content.detail.ip = a.ip();
  content.detail.port = a.port();

  return hasher_(content.repr);
}

// helper
inline std::vector<SocketAddress4> ToOldAddresses(
    const std::vector<NewSockAddress4> &old) {
  std::vector<SocketAddress4> addresses;
  for (const auto &addr : old) {
    addresses.emplace_back(htonl(addr.ip()), htons(addr.port()));
  }

  return addresses;
}

struct Ipv6 {
  Ipv6();
  explicit Ipv6(const in6_addr &addr);
  Ipv6& operator=(const in6_addr &addr);

  std::string ToDebugString() const;
  operator in6_addr() const;
  bool IsGlobal() const;
  bool IsLoopBack() const;

  static Ipv6 Any();
  static Ipv6 Loopback();

  struct in6_addr ip6;
};

// NOTE(liuyong): port should use network order
struct SocketAddress6 {
  SocketAddress6();
  explicit SocketAddress6(const sockaddr_in6 &address);
  SocketAddress6(const struct in6_addr &ip6, uint16_t port,
                 uint32_t scope_id = 0);

  SocketAddress6& operator=(const sockaddr_in6 &addr);

  std::string ToDebugString() const;
  operator sockaddr_in6() const;

  static SocketAddress6 Any();
  static SocketAddress6 Loopback();

  in6_addr ip6;
  uint32_t scope_id;
  uint16_t port;
};

inline bool In6AddrLessThan(const in6_addr &a, const in6_addr &b) {
  auto *left = reinterpret_cast<const uint64_t *>(a.s6_addr32);
  auto *right = reinterpret_cast<const uint64_t *>(b.s6_addr32);

  return left[0] < right[0] || ((left[0] == right[0]) && (left[1] < right[1]));
}

inline bool In6AddrEqualTo(const in6_addr &a, const in6_addr &b) {
  auto *left = reinterpret_cast<const uint64_t *>(a.s6_addr32);
  auto *right = reinterpret_cast<const uint64_t *>(b.s6_addr32);
  return left[0] == right[0] && left[1] == right[1];
}

inline bool operator<(const Ipv6 &a, const Ipv6 &b) {
  return In6AddrLessThan(a.ip6, b.ip6);
}

inline bool operator==(const Ipv6 &a, const Ipv6 &b) {
  return In6AddrEqualTo(a.ip6, b.ip6);
}

inline bool operator!=(const Ipv6 &a, const Ipv6 &b) {
  return !(a == b);
}

inline bool operator<(const SocketAddress6 &l, const SocketAddress6 &r) {
  return In6AddrLessThan(l.ip6, r.ip6) ||
      (In6AddrEqualTo(l.ip6, r.ip6) && (l.port < r.port)) ||
      (In6AddrEqualTo(l.ip6, r.ip6) && (l.port == r.port) &&
      (l.scope_id < r.scope_id));
}

inline bool operator==(const SocketAddress6 &l, const SocketAddress6 &r) {
  return In6AddrEqualTo(l.ip6, r.ip6) && l.port == r.port &&
      l.scope_id == r.scope_id;
}

inline bool operator!=(const SocketAddress6 &l, const SocketAddress6 &r) {
  return !(l == r);
}

inline base::Packer& operator<<(base::Packer &p, const in6_addr &addr) {
  p << addr.s6_addr32[0] << addr.s6_addr32[1] << addr.s6_addr32[2]
    << addr.s6_addr32[3];

  return p;
}

inline base::Packer& operator<<(base::Packer &p, const SocketAddress6 &x) {
  p << x.ip6 << x.scope_id << x.port;
  return p;
}

inline base::Unpacker& operator>>(base::Unpacker &p, in6_addr &addr) {
  p >> addr.s6_addr32[0] >> addr.s6_addr32[1] >> addr.s6_addr32[2]
    >> addr.s6_addr32[3];

  return p;
}

inline base::Unpacker& operator>>(base::Unpacker &p, SocketAddress6 &x) {
  p >> x.ip6 >> x.scope_id >> x.port;
  return p;
}

class GeneralSocketAddress {
 public:
  GeneralSocketAddress();
  explicit GeneralSocketAddress(const sockaddr_storage &addr);

  explicit GeneralSocketAddress(uint32_t ip, uint16_t port);
  explicit GeneralSocketAddress(const NewSockAddress4 &addr);

  // NOTE(liuyong): deprecated
  explicit GeneralSocketAddress(const SocketAddress4 &addr);

  explicit GeneralSocketAddress(const SocketAddress6 &addr);
  explicit GeneralSocketAddress(const SocketAddressUnix &addr);

  GeneralSocketAddress(const GeneralSocketAddress &a);
  ~GeneralSocketAddress();

  GeneralSocketAddress& operator=(const GeneralSocketAddress &a);
  operator sockaddr_storage() const;

  IpVersion GetSocketAddressType() const;
  std::string ToDebugString() const;

  size_t GetHash() const;

  // NOTE(liuyong): returns the port in host order
  uint16_t GetPort() const;

  explicit operator NewSockAddress4() const;

  // NOTE(liuyong): deprecated
  explicit operator SocketAddress4() const;

  explicit operator SocketAddress6() const;
  explicit operator SocketAddressUnix() const;

  void SerializeTo(base::Packer *pkr) const;
  void SerializeFrom(base::Unpacker *unpkr);

  bool LessThan(const GeneralSocketAddress &rhs) const;

 private:
  void Destroy();

 private:
  IpVersion version_;

  union AddressStorage {
    AddressStorage();
    AddressStorage(const AddressStorage &rhs);
    AddressStorage& operator=(const AddressStorage &rhs);

    ~AddressStorage();

    NewSockAddress4 addr4;
    SocketAddress6 addr6;
    SocketAddressUnix addr_unix;
  } addr_;
};

inline GeneralSocketAddress::AddressStorage::AddressStorage() {
}

inline GeneralSocketAddress::AddressStorage::AddressStorage(
    const AddressStorage &rhs) {
  (void)rhs;
}

inline GeneralSocketAddress::AddressStorage&
GeneralSocketAddress::AddressStorage::operator=(const AddressStorage &rhs) {
  (void)rhs;
  return *this;
}

inline GeneralSocketAddress::AddressStorage::~AddressStorage() {
}

inline GeneralSocketAddress::GeneralSocketAddress() {
  version_ = IpVersion::kIp4;
  (void)new (&addr_.addr4)NewSockAddress4;
}

inline GeneralSocketAddress::GeneralSocketAddress(uint32_t ip, uint16_t port)
    : GeneralSocketAddress(NewSockAddress4(ip, port)) {
}

inline GeneralSocketAddress::GeneralSocketAddress(const NewSockAddress4 &a) {
  version_ = IpVersion::kIp4;
  (void)new (&addr_.addr4)NewSockAddress4(a);
}

inline GeneralSocketAddress::GeneralSocketAddress(const SocketAddress4 &a) {
  version_ = IpVersion::kIp4;
  (void)new (&addr_.addr4)NewSockAddress4(a.Ip(), a.Port());
}

inline GeneralSocketAddress::GeneralSocketAddress(const SocketAddress6 &a) {
  version_ = IpVersion::kIp6;
  (void)new (&addr_.addr6)SocketAddress6(a);
}

inline GeneralSocketAddress::GeneralSocketAddress(const SocketAddressUnix &a) {
  version_ = IpVersion::kUnix;
  (void)new (&addr_.addr_unix)SocketAddressUnix(a);
}

inline GeneralSocketAddress::GeneralSocketAddress(
    const GeneralSocketAddress &a) {
  version_ = a.version_;
  switch (version_) {
    case IpVersion::kIp4: {
      (void)new (&addr_.addr4)NewSockAddress4(a.addr_.addr4);
      break;
    }
    case IpVersion::kIp6: {
      (void)new (&addr_.addr6)SocketAddress6(a.addr_.addr6);
      break;
    }
    case IpVersion::kUnix: {
      (void)new (&addr_.addr_unix)SocketAddressUnix(a.addr_.addr_unix);
      break;
    }
    default: assert(false);
  }
}

inline GeneralSocketAddress::~GeneralSocketAddress() {
  Destroy();
}

inline bool operator==(const GeneralSocketAddress &a,
                       const GeneralSocketAddress &b) {
  switch (a.GetSocketAddressType()) {
    case IpVersion::kIp4: {
      return b.GetSocketAddressType() == IpVersion::kIp4
          && NewSockAddress4(a) == NewSockAddress4(b);
    }
    case IpVersion::kIp6: {
      return b.GetSocketAddressType() == IpVersion::kIp6
          && SocketAddress6(a) == SocketAddress6(b);
    }
    case IpVersion::kUnix: {
      return b.GetSocketAddressType() == IpVersion::kUnix
          && SocketAddressUnix(a) == SocketAddressUnix(b);
    }
    default: assert(false);
  }
  return false;
}

inline bool operator!=(const GeneralSocketAddress &a,
                       const GeneralSocketAddress &b) {
  return !(a == b);
}

inline bool operator<(const GeneralSocketAddress &a,
                      const GeneralSocketAddress &b) {
  return a.LessThan(b);
}

inline GeneralSocketAddress::operator NewSockAddress4() const {
  assert(version_ == IpVersion::kIp4);
  return addr_.addr4;
}

inline GeneralSocketAddress::operator SocketAddress4() const {
  assert(version_ == IpVersion::kIp4);
  auto &addr = addr_.addr4;
  return SocketAddress4(htonl(addr.ip()), htons(addr.port()));
}

inline GeneralSocketAddress::operator SocketAddress6() const {
  assert(version_ == IpVersion::kIp6);
  return addr_.addr6;
}

inline GeneralSocketAddress::operator SocketAddressUnix() const {
  assert(version_ == IpVersion::kUnix);
  return addr_.addr_unix;
}

inline uint16_t GeneralSocketAddress::GetPort() const {
  switch (version_) {
    case IpVersion::kIp4: {
      return addr_.addr4.port();
    }
    case IpVersion::kIp6: {
      return ntohs(addr_.addr6.port);
    }
    case IpVersion::kUnix: {
      return 0;
    }
    default: assert(false);
  }
  return 0;
}

inline std::string GeneralSocketAddress::ToDebugString() const {
  switch (version_) {
    case IpVersion::kIp4: {
      return addr_.addr4.ToDebugString();
    }
    case IpVersion::kIp6: {
      return addr_.addr6.ToDebugString();
    }
    case IpVersion::kUnix: {
      return addr_.addr_unix.ToDebugString();
    }
    default: assert(false);
  }

  return "";
}

inline IpVersion GeneralSocketAddress::GetSocketAddressType() const {
  return version_;
}

inline GeneralSocketAddress::operator sockaddr_storage() const {
  sockaddr_storage temp;
  switch (version_) {
    case IpVersion::kIp4: {
      reinterpret_cast<sockaddr_in &>(temp) =
          static_cast<sockaddr_in>(addr_.addr4);
      break;
    }
    case IpVersion::kIp6: {
      reinterpret_cast<sockaddr_in6 &>(temp) = addr_.addr6;
      break;
    }
    case IpVersion::kUnix: {
      reinterpret_cast<sockaddr_un &>(temp) = addr_.addr_unix;
      break;
    }
    default: assert(false);
  }
  return temp;
}

inline void GeneralSocketAddress::Destroy() {
  switch (version_) {
    case IpVersion::kIp4: {
      addr_.addr4.~NewSockAddress4();
      break;
    }
    case IpVersion::kIp6: {
      addr_.addr6.~SocketAddress6();
      break;
    }
    case IpVersion::kUnix: {
      addr_.addr_unix.~SocketAddressUnix();
      break;
    }
    default: assert(false);
  }
}

inline base::Packer& operator<<(
    base::Packer &p, const GeneralSocketAddress &x) {
  x.SerializeTo(&p);
  return p;
}

inline base::Unpacker& operator>>(base::Unpacker &p, GeneralSocketAddress &x) {
  x.SerializeFrom(&p);
  return p;
}

}  // namespace network
}  // namespace agora
