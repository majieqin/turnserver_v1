// Copyright (c) 2014-2017 Agora.io, Inc.
// TODO(liuyong): Detailed description

#include "network_utility.h"

#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cerrno>
#include <cstring>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "address_types.h"

#include "safe_log.h"

namespace agora {
namespace network {

std::vector<uint32_t> ResolveHost(const std::string &host) {
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM | SOCK_STREAM;
  hints.ai_flags = AI_ADDRCONFIG;
  hints.ai_protocol = PF_INET;

  std::vector<uint32_t> ips;
  struct addrinfo *result = nullptr;
  int rc = getaddrinfo(host.c_str(), nullptr, &hints, &result);
  if (rc != 0) {
    SAFE_LOG2(Error) << "Failed to resolve " << host
                     << ", error: " << gai_strerror(rc);
    return ips;
  }

  for (auto rp = result; rp != nullptr; rp = rp->ai_next) {
    auto p = reinterpret_cast<const sockaddr_in *>(rp->ai_addr);
    ips.push_back(ntohl(p->sin_addr.s_addr));
  }

  freeaddrinfo(result);
  return ips;
}

bool IsPublicIp(uint32_t ip) {
  // A: 10.0.0.0 -- 10.255.255.255
  //   127.0.0.0 --127.255.255.255
  // B: 172.16.0.0--172.31.255.255
  //   169.254.0.0--169.254.255.255
  // C: 192.168.0.0--192.168.255.255
  // D: 100.64.0.0 -- 100.127.255.255
  /*
   *                                   10(A)          127(A)          172(B)
   * 169(B)          192.168(C)       100.64(D)
   *                                   10.0.0.0       127.0.0.0
   * 172.16.0.0      169.254.0.0     192.168.0.0      100.64.0.0
   *                                   10.255.255.255 127.255.255.255
   * 172.31.255.255  169.254.255.255 192.168.255.255  100.127.255.255
   */
  static const uint32_t ip_begin[] = {
      0x0a000000, 0x7f000000, 0xac100000, 0xa9fe0000, 0xc0a80000, 0x64400000,
  };

  static const uint32_t ip_end[] = {
      0x0affffff, 0x7fffffff, 0xac1fffff, 0xa9feffff, 0xc0a8ffff, 0x647fffff,
  };

  for (size_t i = 0; i < sizeof(ip_begin) / sizeof(ip_begin[0]); ++i) {
    if (ip >= ip_begin[i] && ip <= ip_end[i]) {
      return false;
    }
  }

  return true;
}

std::vector<uint32_t> LocalAddresses() {
  std::unordered_set<uint32_t> ip_list;

  struct ifaddrs *if_address = nullptr;
  if (getifaddrs(&if_address) == -1) {
    SAFE_LOG2(Error) << "Failed to get all nic info " << strerror(errno);
    return std::vector<uint32_t>();
  }

  for (auto next = if_address; next != nullptr; next = next->ifa_next) {
    if ((next->ifa_addr != nullptr) &&
        (next->ifa_addr->sa_family == AF_INET)) {  // check it is IP4
      auto p = reinterpret_cast<const sockaddr_in *>(next->ifa_addr);
      uint32_t ip = ntohl(p->sin_addr.s_addr);

      if (ip != INADDR_LOOPBACK) {
        ip_list.insert(ip);
      }
    }
  }

  if (if_address != nullptr) {
    freeifaddrs(if_address);
  }

  std::vector<uint32_t> ips;

  for (auto ip : ip_list) {
    ips.push_back(ip);
  }

  return ips;
}

std::vector<uint32_t> LocalPublicAddresses() {
  std::vector<uint32_t> all = LocalAddresses();
  std::vector<uint32_t> left;

  for (uint32_t ip : all) {
    if (IsPublicIp(ip)) left.push_back(ip);
  }

  return left;
}

std::vector<SocketAddress6> LocalIpv6Addresses() {
  std::vector<SocketAddress6> ips;
  ifaddrs *if_address = nullptr;

  if (getifaddrs(&if_address) < 0) {
    SAFE_LOG2(Error) << "Failed to get interface address list: "
                     << strerror(errno);
    return ips;
  }

  for (auto next = if_address; next != nullptr; next = next->ifa_next) {
    if ((next->ifa_addr != nullptr) &&
        (next->ifa_addr->sa_family == AF_INET6)) {
      auto p = reinterpret_cast<const sockaddr_in6 *>(next->ifa_addr);
      Ipv6 a(p->sin6_addr);

      if (!a.IsLoopBack()) {
        ips.push_back(SocketAddress6(*p));
      }
    }
  }

  if (if_address != nullptr) {
    freeifaddrs(if_address);
  }

  return ips;
}

std::vector<SocketAddress6> LocalPublicIpv6Addresses() {
  std::vector<SocketAddress6> all = LocalIpv6Addresses();
  std::vector<SocketAddress6> left;
  for (auto &addr : all) {
    if (Ipv6(addr.ip6).IsGlobal()) {
      left.push_back(addr);
    }
  }

  return left;
}

}  // namespace network
}  // namespace agora
