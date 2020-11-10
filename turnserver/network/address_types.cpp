// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
// Network address types

#include "address_types.h"

#include <arpa/inet.h>

#include <algorithm>
#include <cinttypes>
#include <cstdio>
#include <cstring>

#include "city_hash.h"

namespace agora {
namespace network {

SocketAddressUnix& SocketAddressUnix::operator=(const sockaddr_un &addr) {
  path = std::string(addr.sun_path);

  return *this;
}

std::string SocketAddressUnix::Path() const {
  return path;
}

std::string SocketAddressUnix::ToDebugString() const {
  return Path();
}

SocketAddressUnix::operator sockaddr_un() const {
  sockaddr_un addr;
  ::memset(&addr, 0, sizeof(addr));

  addr.sun_family = AF_UNIX;
  snprintf(addr.sun_path, sizeof(addr.sun_path) - 1, "%s", path.c_str());

  return addr;
}

static std::string SockAddr4ToString(const sockaddr_in &addr) {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%s:%" PRIu16, inet_ntoa(addr.sin_addr),
           ntohs(addr.sin_port));

  return std::string(buffer);
}

SocketAddress4& SocketAddress4::operator=(const sockaddr_in &addr) {
  ip = addr.sin_addr.s_addr;
  port = addr.sin_port;

  return *this;
}

std::string SocketAddress4::ToDebugString() const {
  return SockAddr4ToString(static_cast<sockaddr_in>(*this));
}

SocketAddress4::operator sockaddr_in() const {
  sockaddr_in addr;
  ::memset(&addr, 0, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = ip;

  return addr;
}

std::string NewSockAddress4::ToDebugString() const {
  return SockAddr4ToString(static_cast<sockaddr_in>(*this));
}

NewSockAddress4::operator sockaddr_in() const {
  sockaddr_in addr;
  ::memset(&addr, 0, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port_);
  addr.sin_addr.s_addr = htonl(ip_);

  return addr;
}

Ipv6::Ipv6() {
}

Ipv6::Ipv6(const in6_addr &addr) {
  ip6 = addr;
}

Ipv6& Ipv6::operator=(const in6_addr &addr) {
  ip6 = addr;
  return *this;
}

std::string Ipv6::ToDebugString() const {
  char buf[128];
  inet_ntop(AF_INET6, &ip6, buf, sizeof(buf));
  return std::string(buf);
}

Ipv6::operator in6_addr() const {
  return ip6;
}

bool Ipv6::IsGlobal() const {
  // FIXME(liuyong)
  uint32_t x = 0;
  memcpy(&x, ip6.s6_addr, sizeof(x));
  return ((x & htonl(0x70000000)) == htonl(0x20000000));
}

bool Ipv6::IsLoopBack() const {
  return In6AddrEqualTo(ip6, in6addr_loopback);
}

Ipv6 Ipv6::Any() {
  return Ipv6(in6addr_any);
}

Ipv6 Ipv6::Loopback() {
  return Ipv6(in6addr_loopback);
}

SocketAddress6::SocketAddress6() {
  ip6 = IN6ADDR_LOOPBACK_INIT;

  scope_id = 0;
  port = 0;
}

SocketAddress6::SocketAddress6(const sockaddr_in6 &a)
    : ip6(a.sin6_addr), scope_id(a.sin6_scope_id), port(a.sin6_port) {
}

SocketAddress6::SocketAddress6(
    const in6_addr &a, uint16_t prt, uint32_t local_scope_id)
    : ip6(a), scope_id(local_scope_id), port(prt) {
}

SocketAddress6& SocketAddress6::operator=(const sockaddr_in6 &addr) {
  ip6 = addr.sin6_addr;
  scope_id = addr.sin6_scope_id;
  port = addr.sin6_port;

  return *this;
}

std::string SocketAddress6::ToDebugString() const {
  char buf[128];
  buf[0] = '[';

  inet_ntop(AF_INET6, &ip6, buf + 1, sizeof(buf) - 1);
  size_t len = strlen(buf);

  snprintf(buf + len, sizeof(buf) - len, "]:%" PRIu16, ntohs(port));

  return std::string(buf);
}

SocketAddress6::operator sockaddr_in6() const {
  sockaddr_in6 addr;
  ::memset(&addr, 0, sizeof(addr));

  addr.sin6_family = AF_INET6;
  addr.sin6_addr = ip6;
  addr.sin6_scope_id = scope_id;
  addr.sin6_port = port;

  return addr;
}

SocketAddress6 SocketAddress6::Any() {
  static const SocketAddress6 any(in6addr_any, 0);
  return any;
}

SocketAddress6 SocketAddress6::Loopback() {
  static const SocketAddress6 loopback(in6addr_loopback, 0);
  return loopback;
}

GeneralSocketAddress::GeneralSocketAddress(const sockaddr_storage &a) {
  switch (a.ss_family) {
    case AF_INET: {
      version_ = IpVersion::kIp4;

      auto &rhs = reinterpret_cast<const sockaddr_in &>(a);
      (void)new (&addr_.addr4)NewSockAddress4(rhs);
      break;
    }
    case AF_INET6: {
      version_ = IpVersion::kIp6;

      auto &rhs = reinterpret_cast<const sockaddr_in6 &>(a);
      (void)new (&addr_.addr6)SocketAddress6(rhs);
      break;
    }
    case AF_UNIX: {
      version_ = IpVersion::kUnix;

      auto &rhs = reinterpret_cast<const sockaddr_un &>(a);
      (void)new (&addr_.addr_unix)SocketAddressUnix(rhs);
      break;
    }
    default: assert(false);
  }
}

void GeneralSocketAddress::SerializeTo(base::Packer *pkr) const {
  auto &packing = *pkr;
  packing << static_cast<uint16_t>(version_);

  switch (version_) {
    case IpVersion::kIp4: {
      packing << addr_.addr4;
      break;
    }
    case IpVersion::kIp6: {
      packing << addr_.addr6;
      break;
    }
    case IpVersion::kUnix: {
      packing << addr_.addr_unix;
      break;
    }
    default: assert(false);
  }
}

GeneralSocketAddress& GeneralSocketAddress::operator=(
    const GeneralSocketAddress &address) {
  if (this == &address) {
    return *this;
  }

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

  version_ = address.version_;

  switch (version_) {
    case IpVersion::kIp4: {
      (void)new (&addr_.addr4)NewSockAddress4(address.addr_.addr4);
      break;
    }
    case IpVersion::kIp6: {
      (void)new (&addr_.addr6)SocketAddress6(address.addr_.addr6);
      break;
    }
    case IpVersion::kUnix: {
      (void)new (&addr_.addr_unix)SocketAddressUnix(address.addr_.addr_unix);
      break;
    }
    default: assert(false);
  }

  return *this;
}

void GeneralSocketAddress::SerializeFrom(base::Unpacker *p) {
  auto &unpacker = *p;
  uint16_t ver = 0;
  unpacker >> ver;

  IpVersion version = static_cast<IpVersion>(ver);

  switch (version) {
    case IpVersion::kIp4: {
      NewSockAddress4 temp;
      unpacker >> temp;

      Destroy();

      version_ = version;
      (void) new (&addr_.addr4)NewSockAddress4(std::move(temp));
      break;
    }
    case IpVersion::kIp6: {
      SocketAddress6 temp;
      unpacker >> temp;

      Destroy();

      version_ = version;
      (void) new (&addr_.addr6)SocketAddress6(std::move(temp));
      break;
    }
    case IpVersion::kUnix: {
      SocketAddressUnix temp;
      unpacker >> temp;

      Destroy();

      version_ = version;
      (void) new (&addr_.addr_unix)SocketAddressUnix(std::move(temp));
      break;
    }
    default: assert(false);
  }
}

size_t GeneralSocketAddress::GetHash() const {
  if (version_ == IpVersion::kIp4) {
    auto b = (static_cast<uint64_t>(addr_.addr4.ip()) << 16) +
        addr_.addr4.port();

    return std::hash<uint64_t>()(b);
  }

  static_assert(sizeof(in6_addr) == 16, "Unaligned layout");
  static_assert(sizeof(addr_.addr6) == 24, "Unaligned layout");

  if (version_ == IpVersion::kIp6) {
    auto p = reinterpret_cast<const char *>(&addr_.addr6);
#if __SIZEOF_SIZE_T__ == 4
    return base::CityHash32(p, 22);
#elif __SIZEOF_SIZE_T__ == 8
    return base::CityHash64(p, 22);
#else
    static_assert(false, "Unaligned layout");
#endif
  }

  assert(version_ == IpVersion::kUnix);
  return std::hash<std::string>()(addr_.addr_unix.Path());
}

bool GeneralSocketAddress::LessThan(const GeneralSocketAddress &rhs) const {
  if (version_ < rhs.version_) {
    return true;
  } else if (version_ > rhs.version_) {
    return false;
  }

  assert(version_ == rhs.version_);
  switch (version_) {
    case IpVersion::kIp4:
      return addr_.addr4 < rhs.addr_.addr4;
    case IpVersion::kIp6:
      return addr_.addr6 < rhs.addr_.addr6;
    case IpVersion::kUnix:
      return addr_.addr_unix < rhs.addr_.addr_unix;
    default:
      break;
  }

  assert(false);
  return false;
}

}  // namespace network
}  // namespace agora
