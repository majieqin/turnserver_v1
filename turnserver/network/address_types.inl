inline NewSockAddress4::NewSockAddress4(uint32_t ip, uint16_t port) {
  ip_ = ip;
  port_ = port;
}

inline NewSockAddress4::NewSockAddress4(const SocketAddress4 &address) {
  ip_ = address.Ip();
  port_ = address.Port();
}

inline NewSockAddress4::NewSockAddress4(const sockaddr_in &addr) {
  ip_ = ntohl(addr.sin_addr.s_addr);
  port_ = ntohs(addr.sin_port);
}

inline NewSockAddress4& NewSockAddress4::operator=(const SocketAddress4 &addr) {
  ip_ = addr.Ip();
  port_ = addr.Port();
  return *this;
}

inline NewSockAddress4& NewSockAddress4::operator=(const sockaddr_in &addr) {
  ip_ = ntohl(addr.sin_addr.s_addr);
  port_ = ntohs(addr.sin_port);
  return *this;
}

inline base::Packer& operator<<(base::Packer &pkr, const NewSockAddress4 &addr) {
  addr.SerializeTo(&pkr);
  return pkr;
}

inline base::Unpacker& operator>>(base::Unpacker &pkr, NewSockAddress4 &addr) {
  addr.SerializeFrom(&pkr);
  return pkr;
}

inline uint32_t NewSockAddress4::ip() const {
  return ip_;
}

inline uint16_t NewSockAddress4::port() const {
  return port_;
}

inline void NewSockAddress4::SetIp(uint32_t ip) {
  ip_ = ip;
}

inline void NewSockAddress4::SetPort(uint16_t port) {
  port_ = port;
}

inline void NewSockAddress4::SerializeTo(base::Packer *p) const {
  *p << static_cast<uint32_t>(htonl(ip_))
     << static_cast<uint16_t>(htons(port_));
}

inline void NewSockAddress4::SerializeFrom(base::Unpacker *p) {
  uint32_t ip = 0;
  uint16_t port = 0;
  *p >> ip >> port;

  ip_ = ntohl(ip);
  port_ = ntohs(port);
}

inline bool operator<(const NewSockAddress4 &a, const NewSockAddress4 &b) {
  return a.ip() < b.ip() || ((a.ip() == b.ip()) && (a.port() < b.port()));
}

inline bool operator>(const NewSockAddress4 &a, const NewSockAddress4 &b) {
  return a.ip() > b.ip() || ((a.ip() == b.ip()) && (a.port() > b.port()));
}

inline bool operator==(const NewSockAddress4 &a, const NewSockAddress4 &b) {
  return a.ip() == b.ip() && a.port() == b.port();
}

inline bool operator!=(const NewSockAddress4 &a, const NewSockAddress4 &b) {
  return !(a == b);
}
