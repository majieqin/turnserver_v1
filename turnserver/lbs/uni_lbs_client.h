// Copyright (c) 2014-2017 Agora.io, Inc.
// A class helping to communnicating with multiple unified lbs servers,
//

#pragma once  // NOLINT(build/header_guard)

#include <netinet/in.h>

#include <cstdint>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

#include "event2/event.h"

#include "timer.h"
#include "address_types.h"
#include "uni_lbs_connection.h"

namespace agora {
namespace base {
class Unpacker;
class EventLoop;
}  // namespace base

namespace lbs {

class UniLbsConnection;

class UniLbsClient : private UniLbsEventCallback {
 public:
  struct Visitor;

  // |port| should be host-ordered
  UniLbsClient(const std::string &host, const std::string &service_name,
               uint16_t lbs_port, const std::string &release_version,
               base::EventLoop *loop, Visitor *callback);

  UniLbsClient(const std::string &host, const std::string &service_name,
               uint16_t lbs_port, const std::string &release_version,
               event_base *loop, Visitor *callback);

  virtual ~UniLbsClient();

  void SetReloadingInterval(uint32_t seconds = 600u);

  void UpdateLbsAddresses(
      const std::vector<network::SocketAddress4> &new_addresses);

  void UpdateLbsAddresses(
      const std::vector<network::NewSockAddress4> &new_addresses);

  // After the function is invoked, |Connect| will be triggered automatically.
  // |service_port| should be host-ordered
  void SetServiceInformation(uint32_t service_id, uint32_t service_port);
  void DoChannelCreate(uint32_t vid, uint32_t cid, const std::string &name);
  void DoChannelDestroy(uint32_t vid, uint32_t cid, const std::string &name);

 private:
  typedef std::unique_ptr<UniLbsConnection> UniLbsConnectionPtr;

  virtual void OnLbsConnected(UniLbsConnection *c) OVERRIDE;
  virtual void OnLbsDisconnected(UniLbsConnection *c) OVERRIDE;
  virtual void OnLbsMessageReceived(UniLbsConnection *c, base::Unpacker *p,
                                     uint16_t uri) OVERRIDE;

  virtual void OnUpdateEdgeServerLoadQuota(
      UniLbsConnection *c, uint32_t max_users) OVERRIDE;

  virtual void OnJoinReplied(
      UniLbsConnection *c, const protocol::lbs::DetailInfo2Map &details,
      std::vector<ChannelInfo> *channels) OVERRIDE;

  virtual void OnEdgeServerPerformanceRequired(
      uint32_t *users, uint32_t *cpu_percentage, uint32_t *rx_kbps,
      uint32_t *tx_kbps) OVERRIDE;

  void ReloadLbsHosts();
  void ReloadLbsAddresses(
      const std::vector<network::NewSockAddress4> &new_addresses);

 private:
  std::string lbs_host_;
  uint16_t lbs_port_;

  std::string service_name_;
  uint32_t service_id_;
  uint32_t service_port_;

  uint32_t reloading_interval_;
  event_base *loop_;

  Visitor *callback_;

  base::RepeatedTimer timer_;

  // host-ordered ip --> UniLbsConnection
  std::unordered_map<uint32_t, UniLbsConnectionPtr> connections_;

  std::string release_version_;
};

struct UniLbsClient::Visitor {
  virtual void OnEdgeServerPerformanceRequired(
      uint32_t *user_count, uint32_t *cpu_percentage,
      uint32_t *rx_kbps, uint32_t *tx_kbps) = 0;

  virtual void OnFullChannelListRequired(
      UniLbsConnection *c, std::vector<ChannelInfo> *channels);

  virtual void OnLbsConnected(UniLbsConnection *c);

  virtual void OnLbsDisconnected(UniLbsConnection *c);

  virtual void OnAllLbsDisconnected();

  // Received a message from |UniLbsConnection|
  virtual void OnLbsMessageReceived(UniLbsConnection *c, base::Unpacker *p,
      uint16_t uri);

  virtual void OnUpdateEdgeServerLoadQuota(uint32_t max_users);

 protected:
  virtual ~Visitor() {}
};

}  // namespace lbs
}  // namespace agora
