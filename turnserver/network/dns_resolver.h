// Copyright (c) 2014-2017 Agora.io, Inc.
// A asynchrounous dns resolver using libevent dns api

#pragma once  // NOLINT(build/header_guard)

#include <event2/util.h>

#include <string>
#include <vector>
#include <memory>

#include "media_server_library/base/timer.h"

struct event_base;
struct evdns_getaddrinfo_request;
struct evdns_base;

namespace agora {
namespace network {

class DNSResolver;
class DNSResolverDeleter;

struct DNSResolverDeleter {
  void operator()(DNSResolver *p);
};

typedef std::unique_ptr<network::DNSResolver, DNSResolverDeleter>
        DNSResolverPtr;
typedef std::vector<std::string> ResolvedAddresses;

struct DNSResolverCallback {
  virtual void OnResolved(DNSResolver *dns_resolver,
      const ResolvedAddresses &addrs) = 0;

 protected:
  virtual ~DNSResolverCallback() {}
};

class DNSResolver {
  friend class DNSResolverDeleter;
 public:
  enum LogMode {
    kVerbose = 0,
    kSilent = 1,
  };
  static DNSResolverPtr Create(event_base *base, const char *domain_name,
                               DNSResolverCallback *callback,
                               LogMode mode = kVerbose);
  std::string GetDomainName() const;
  bool RequestResolve(bool use_getaddrinfo = true);
  bool IsClosed() const;
  void Close();

 private:
  void CloseAndDestroy();
  DNSResolver(DNSResolverCallback *callback, const char *domain_name,
              LogMode mode = kVerbose);
  ~DNSResolver();
  bool Init(event_base* base);
  void Destroy();

  void OnDNSResolvedMainInternal(int result, char type, int count, int ttl,
      void *addrs);
  void OnDNSResolvedMain(int result, char type, int count, int ttl,
      void *addrs);
  void OnDNSResolvedGai(int err, struct evutil_addrinfo *ai);
  void OnDNSResolvedGaiInternal(int err, struct evutil_addrinfo *ai);

  static void DNSResolvedCallbackMain(int result, char type, int count, int ttl,
      void *addrs, void *orig);

  static void DNSResolvedCallbackGai(int err, struct evutil_addrinfo *ai,
      void *arg);

 private:
  event_base *event_base_;
  struct evdns_base *evdns_base_;
  std::string domain_name_;
  DNSResolverCallback *dns_resolver_callback_;
  LogMode mode_;
  size_t requesting_count_;
  bool closed_;
  bool is_in_processing_;
};

inline std::string DNSResolver::GetDomainName() const {
  return domain_name_;
}

inline bool DNSResolver::IsClosed() const {
  return closed_;
}

}  // namespace network
}  // namespace agora
