// Copyright (c) 2014-2017 Agora.io, Inc.
// A class helping to communnicating with a single LBS server,
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "event2/event.h"

namespace agora {
namespace base {
class Packet;
class Unpacker;
}  // namespace base

namespace lbs {

class UniLbsClient;
class UniLbsConnection;
struct ChannelInfo;

class UniLbsClientInterface {
 public:
  struct Callback;

  // |port| should be host-ordered
  UniLbsClientInterface(
      const std::string &host, const std::string &service_name,
      uint16_t lbs_port, event_base *loop,
      Callback *callback);

  virtual ~UniLbsClientInterface();

  void SetReloadingInterval(uint32_t seconds = 600u);

  // After the function is invoked, |Connect| will be triggered automatically.
  // |service_port| should be host-ordered
  void SetServiceInformation(uint32_t service_id, uint32_t service_port);
  void DoChannelCreate(uint32_t vid, uint32_t cid, const std::string &name);
  void DoChannelDestroy(uint32_t vid, uint32_t cid, const std::string &name);

 private:
  std::unique_ptr<UniLbsClient> client_;
};

struct UniLbsClientInterface::Callback {
  virtual void OnLbsConnected(UniLbsConnection *c) = 0;

  virtual void OnLbsDisconnected(UniLbsConnection *c) = 0;

  // Received a message from |UniLbsConnection|
  virtual void OnLbsMessageReceived(UniLbsConnection *c, base::Unpacker *p,
                                    uint16_t uri) = 0;

  virtual void OnFullChannelListRequired(
      UniLbsConnection *c, std::vector<ChannelInfo> *channels) = 0;

  virtual void OnEdgeServerPerformanceRequired(
      uint32_t *users, uint32_t *cpu_percentage, uint32_t *rx_kbps,
      uint32_t *tx_kbps) = 0;

  virtual void OnUpdateEdgeServerLoadQuota(uint32_t max_users) = 0;
  virtual void OnAllLbsDisconnected() = 0;
  virtual ~Callback() = 0;
};

}  // namespace lbs
}  // namespace agora
