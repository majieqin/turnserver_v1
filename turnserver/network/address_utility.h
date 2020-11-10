// Copyright (c) 2017-2017 Agora.io, Inc.
// media-server@agora.io
// Address utilities

#pragma once  // NOLINT(build/header_guard)

#include <arpa/inet.h>
#include <netinet/in.h>

#include <string>
#include <vector>

#include "event2/bufferevent.h"

#include "address_types.h"
#include "../base/packet_helper.h"

namespace agora {
namespace network {

// |ip| should be network-ordered
struct OldIspIpPair {
  std::string isp;
  uint32_t ip;
};

typedef std::vector<OldIspIpPair> OldIspIpPairList;

// |ip| network-ordered, while |port| host-ordered
NEW_DECLARE_PACKABLE_3_START(OldIspServiceAddress,
    std::string, isp, uint32_t, ip, uint16_t, port)
  std::string ToDebugString() const;
NEW_DECLARE_STRUCT_END

typedef std::vector<OldIspServiceAddress> OldIspServiceAddressList;

NEW_DECLARE_PACKABLE_2(OldAddressInfo, std::string, ip, uint16_t, port)
typedef std::vector<OldAddressInfo> OldAddressList;

sockaddr_in ToAddress(uint32_t ip, uint16_t port);

uint32_t IpToInt(const std::string &ip);
std::vector<uint8_t> IpToVector(const std::string &ip);
std::vector<uint8_t> Ipv4ToVector(const std::string &ip);
std::vector<uint8_t> Ipv6ToVector(const std::string &ip);
std::string IpToString(uint32_t ip);
std::string IpToString(const sockaddr_in &peer_addr);
std::string IpToString(const sockaddr_in6 &peer_addr);
std::string IpToString(const sockaddr_storage &peer_addr);

std::string AddressToString(const sockaddr_in &peer_addr);
std::string AddressToString(const sockaddr_in6 &peer_addr);
std::string AddressToString(const sockaddr_storage &peer_addr);

std::string AddressToString(uint32_t ip, uint16_t port);
std::string AddressToString(const std::string &ip, uint16_t port);

// |str| must have the form like: 1.2.3.4.5:3333
bool StringToAddress(const std::string &str, network::SocketAddress4 *addr);

// |str| must have the form like: 1.2.3.4.5:3333
bool StringToAddress(const std::string &str, network::NewSockAddress4 *addr);

uint32_t AddressToIp(const sockaddr_in &addr);
std::vector<uint8_t> AddressToIp(const sockaddr_in6 &addr);
uint32_t AddressToIp(const sockaddr_storage &addr);

bool AddressEquals(const sockaddr_in &l, const sockaddr_in &r);
bool AddressEquals(const sockaddr_in6 &l, const sockaddr_in6 &r);
bool AddressEquals(const sockaddr_storage &l, const sockaddr_storage &r);

sockaddr_in FdToAddress(int s);
sockaddr_storage FdToAddress2(int s);
std::string BevToAddress(bufferevent *bev);

}  // namespace network
}  // namespace agora
