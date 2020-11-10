// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
//
// utilities for string operations

#pragma once  // NOLINT(build/header_guard)

#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace agora {
namespace base {

std::string StringToHex(const std::vector<uint8_t> &in);
std::string StringToHex(const std::string &in);
std::string StringToUpper(const std::string &src);
std::string StringToLower(const std::string &src);

std::vector<std::string> SplitString(const std::string &s, char delim);

template<typename _Ty1, typename _Ty2>
std::ostream &operator<<(std::ostream &out,  // NOLINT(runtime/references)
                         const std::pair<_Ty1, _Ty2> &v) {
  out << v.first << ":" << v.second;
  return out;
}

template<typename T>
inline std::string MakeString(const T &c, const std::string &separator) {
  std::ostringstream oss;
  std::string s("");

  for (const auto &v : c) {
    oss << s << v;
    s = separator;
  }

  return oss.str();
}

template<typename T, typename F>
inline std::string MakeString(const T &c, const std::string &delim, F f) {
  std::ostringstream oss;
  std::string s;

  for (const auto &v : c) {
    oss << s << f(v);
    s = delim;
  }

  return oss.str();
}
}  // namespace base
}  // namespace agora
