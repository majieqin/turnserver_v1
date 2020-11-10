// Copyright (c) 2014-2017 Agora.io, Inc.
// A class helping to communnicating with multiple config services,
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "base/cpp_feature.h"
#include "base/timer.h"

#include "network/config_connection.h"

struct event_base;

namespace agora {
namespace base {

class EventLoop;

}  // namespace base

namespace network {

struct ConfigMessageCallback {
  // Received a config string from |ConfigServer|
  virtual void OnConfigLoaded(ConfigConnection *c, uint32_t version,
                              const std::string &config);
  virtual void OnConfigLoaded2(ConfigConnection *c, uint32_t version,
                               const std::string &service_name,
                               const std::string &config);
  virtual void OnWildMatchConfigLoaded(
      ConfigConnection *c, const std::string &service_name,
      std::vector<std::string> update_idcs,
      std::vector<std::string> all_idcs,  // sorted by relativity
      const protocol::config_server::ConfigInfoMap &all_configs);

 protected:
  virtual ~ConfigMessageCallback() = default;
};

class ConfigAgent : private ConfigConnectionCallback,
                    private base::NonCopyOrMovable {
 public:
  // NOTE(liuyong): IP addresses should be in host byte orde.r
  ConfigAgent(base::EventLoop *loop, const std::string &host,
              const std::string &service_name, uint16_t port,
              ConfigMessageCallback *callback, bool enable_wild_match = false);

  ConfigAgent(event_base *net, const std::string &host,
              const std::string &service_name, uint16_t port,
              ConfigMessageCallback *callback, bool enable_wild_match = false);

  virtual ~ConfigAgent();

  // Once |Start| is called, configuration pulling will be triggered about
  // every |interval| seconds.
  bool Start(uint32_t service_id, const std::string &idc,
             uint32_t initial_version = 0, uint32_t interval = 300);

  void RequestLatestConfig(bool update_hosts = false);

 private:
  typedef std::unique_ptr<ConfigConnection> ConfigConnectionPtr;
  typedef std::map<uint32_t, ConfigConnectionPtr> ConfigConnectionMap;

  virtual void OnConfigLoaded(ConfigConnection *from, const std::string &config,
                              uint32_t version) OVERRIDE;

  virtual void OnWildMatchConfigLoaded(
      ConfigConnection *from,
      const protocol::config_server::ConfigInfoMap &configs,
      const std::string &my_idc) OVERRIDE;

  bool TimeToRequestLatestConfig();
  bool TimeToUpdateHosts();

  void UpdateConfigServices();

  void InternalRequestLatestConfig();
  void InternalUpdateHosts();

 private:
  const std::string host_;
  std::string service_name_;
  std::string idc_;

  uint16_t port_;

  event_base *loop_;
  ConfigMessageCallback *callback_;

  int32_t next_update_tick_;
  int32_t prev_resolving_tick_;

  uint32_t interval_ms_;
  uint32_t service_id_;
  uint64_t version_;

  ConfigConnectionMap clients_;
  base::RepeatedTimer timer_;

  bool enable_wild_match_;

  protocol::config_server::ConfigInfoMap local_configs_;
};

}  // namespace network
}  // namespace agora
