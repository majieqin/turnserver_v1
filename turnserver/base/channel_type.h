// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
//
// TODO(liuyong): A more detailed description is required.

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <utility>

namespace agora {
namespace base {

typedef uint32_t channel_id_t;
#define CID_FORMAT "%u"

// TODO(liuyong): compress |channels| before sending them to the vosync server.
typedef std::pair<uint32_t, std::string> ChannelInfo;
typedef std::pair<ChannelInfo, uint32_t> LocalChannelInfo;

class ChannelType {
  friend bool operator ==(const ChannelType &, const ChannelType &);
  friend bool operator !=(const ChannelType &, const ChannelType &);
  friend bool operator <(const ChannelType &, const ChannelType &);

 public:
  ChannelType();
  explicit ChannelType(const ChannelInfo &c);

  // Time-consuming CityHash calculation only occurs in this constructor.
  ChannelType(uint32_t vid, const std::string &cname);

  ~ChannelType();

  ChannelType(const ChannelType &) = default;
  ChannelType& operator=(const ChannelType &) = default;

  ChannelType(ChannelType &&) = default;
  ChannelType& operator=(ChannelType &&) = default;

  ChannelInfo unique_name() const;
  uint64_t GetHash() const;
  uint32_t vendor_id() const;
  const std::string& channel_name() const;

 private:
  void CalculateHash();

 private:
  uint32_t vid_;
  std::string cname_;
  uint64_t hash_val_;
};

inline ChannelType::~ChannelType() {
}

inline uint64_t ChannelType::GetHash() const {
  return hash_val_;
}

inline uint32_t ChannelType::vendor_id() const {
  return vid_;
}

inline const std::string& ChannelType::channel_name() const {
  return cname_;
}

inline bool operator ==(const ChannelType &a, const ChannelType &b) {
  return a.hash_val_ == b.hash_val_ && a.vid_ == b.vid_ &&
      a.cname_ == b.cname_;
}

inline bool operator !=(const ChannelType &a, const ChannelType &b) {
  return !(a == b);
}

inline bool operator <(const ChannelType &a, const ChannelType &b) {
  return a.hash_val_ < b.hash_val_ || (a.hash_val_ == b.hash_val_ &&
      a.vid_ < b.vid_) || (a.hash_val_ == b.hash_val_ &&
      a.vid_ == b.vid_ && a.cname_ < b.cname_);
}

inline ChannelInfo ChannelType::unique_name() const {
  return ChannelInfo(vid_, cname_);
}

std::ostream& operator<<(std::ostream &out, const ChannelType &c);
std::ostream& operator<<(std::ostream &out, const ChannelInfo &c);

struct ChannelHasher {
  size_t operator() (const ChannelType &a) const {
    return a.GetHash();
  }
};

}  // namespace base
}  // namespace agora
