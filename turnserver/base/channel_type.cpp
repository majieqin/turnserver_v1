// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
//
// TODO(liuyong): A more detailed description is required.

#include "base/channel_type.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "base/city_hash.h"

namespace agora {
namespace base {

ChannelType::ChannelType() {
  hash_val_ = 0;
  vid_ = 0;
}

ChannelType::ChannelType(const ChannelInfo &c)
    : vid_(c.first), cname_(c.second) {
  CalculateHash();
}

ChannelType::ChannelType(uint32_t vid, const std::string &cname)
    : vid_(vid), cname_(cname) {
  CalculateHash();
}

void ChannelType::CalculateHash() {
  std::vector<char> buf(cname_.size() + sizeof(uint32_t));
  std::copy(cname_.begin(), cname_.end(), buf.begin());

  union {
    char content[sizeof(uint32_t)];
    uint32_t vid;
  };

  vid = vid_;
  std::copy(content, content + sizeof(vid), buf.begin() + cname_.size());
  hash_val_ = CityHash64(&buf[0], buf.size());
}

std::ostream& operator<<(std::ostream &out, const ChannelType &c) {
  out << "(vid: " << c.vendor_id() << ", channel: " << c.channel_name() << ")";
  return out;
}

std::ostream& operator<<(std::ostream &out, const ChannelInfo &c) {
  out << "(" << c.first << ", " << c.second << ")";
  return out;
}

}  // namespace base
}  // namespace agora
