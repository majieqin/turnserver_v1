// Copyright (c) 2014-2017 Agora.io, Inc.
// TODO(liuyong): Detailed description

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <string>
#include <vector>

#include "address_types.h"

namespace agora {
namespace network {

// Returns a list of host-ordered ip
std::vector<uint32_t> ResolveHost(const std::string &host);

// Returns a list of host-ordered local ip addresses
std::vector<uint32_t> LocalAddresses();

// Returns a list of host-ordered local ip except private ones
std::vector<uint32_t> LocalPublicAddresses();

// Returns true if host-ordered |ip| is public
bool IsPublicIp(uint32_t ip);

// Returns a list of local ip6
std::vector<SocketAddress6> LocalIpv6Addresses();

// Returns a list of host-ordered local ip except private ones
std::vector<SocketAddress6> LocalPublicIpv6Addresses();

}  // namespace network
}  // namespace agora
