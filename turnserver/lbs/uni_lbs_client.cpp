// Copyright (c) 2014-2017 Agora.io, Inc.
// Implementation of UniLbsClient
//

#include "uni_lbs_client.h"

#include <netinet/in.h>

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>

#include "event_loop.h"
#include "timer.h"
#include "safe_log.h"
#include "network_utility.h"

namespace agora {
namespace lbs {

using std::string;
using std::vector;
using std::unordered_map;

using base::RepeatedTimer;
using network::SocketAddress4;
using network::NewSockAddress4;

void UniLbsClient::Visitor::OnFullChannelListRequired(
    UniLbsConnection *c, std::vector<ChannelInfo> *channels) {
  (void)c;
  (void)channels;
}

void UniLbsClient::Visitor::OnLbsConnected(UniLbsConnection *) {}

void UniLbsClient::Visitor::OnLbsDisconnected(UniLbsConnection *) {}

void UniLbsClient::Visitor::OnAllLbsDisconnected() {}
  // Received a message from |UniLbsConnection|
void UniLbsClient::Visitor::OnLbsMessageReceived(
    UniLbsConnection *c, base::Unpacker *p, uint16_t uri) {
  (void)c;
  (void)p;
  (void)uri;
}

void UniLbsClient::Visitor::OnUpdateEdgeServerLoadQuota(uint32_t max_users) {
  (void)max_users;
}

UniLbsClient::UniLbsClient(const string &host,
                           const string &service_name,
                           uint16_t lbs_port,
                           const string &release_version,
                           base::EventLoop *loop, Visitor *callback)
  : UniLbsClient(host, service_name, lbs_port, release_version,
                 loop->GetEventBase(), callback) {
}

UniLbsClient::UniLbsClient(const string &host,
                           const string &service_name,
                           uint16_t lbs_port,
                           const string &release_version,
                           event_base *loop, Visitor *callback) {
  lbs_host_ = host;
  service_name_ = service_name;
  lbs_port_ = lbs_port;
  loop_ = loop;
  callback_ = callback;

  service_id_ = 0;
  service_port_ = 0;

  reloading_interval_ = 600u;
  release_version_ = release_version;
}

UniLbsClient::~UniLbsClient() {}

void UniLbsClient::SetReloadingInterval(uint32_t seconds) {
  reloading_interval_ = std::max<uint32_t>(seconds, 100u);
}

void UniLbsClient::UpdateLbsAddresses(
    const std::vector<network::SocketAddress4> &new_addresses) {
  std::vector<network::NewSockAddress4> addresses;
  auto converter = [](const network::SocketAddress4 &a) {
    return network::NewSockAddress4(a);
  };

  std::transform(new_addresses.begin(), new_addresses.end(),
                 std::back_inserter(addresses), converter);

  UpdateLbsAddresses(addresses);
}

void UniLbsClient::UpdateLbsAddresses(
    const std::vector<network::NewSockAddress4> &new_addresses) {
  timer_ = base::RepeatedTimer();

  if (service_id_) {
    ReloadLbsAddresses(new_addresses);
  }
}

void UniLbsClient::SetServiceInformation(uint32_t service_id,
                                         uint32_t service_port) {
  service_id_ = service_id;
  service_port_ = service_port;

  ReloadLbsHosts();

  // Ready to start a timer to invoke |ReloadLbsHosts| timely
  timer_ = RepeatedTimer(loop_, reloading_interval_ * 1000,
                         &UniLbsClient::ReloadLbsHosts, this);
}

void UniLbsClient::DoChannelCreate(uint32_t vid, uint32_t cid,
                                const string &name) {
  for (auto &p : connections_) {
    ChannelInfo channel = {
      vid,
      cid,
      name,
    };

    p.second->DoChannelCreate(channel);
  }
}

void UniLbsClient::DoChannelDestroy(uint32_t vid, uint32_t cid,
                                 const string &name) {
  for (auto &p : connections_) {
    ChannelInfo channel = {
      vid,
      cid,
      name,
    };

    p.second->DoChannelDestroy(channel);
  }
}

void UniLbsClient::OnLbsConnected(UniLbsConnection *c) { (void)c; }

void UniLbsClient::OnLbsDisconnected(UniLbsConnection *c) { (void)c; }

void UniLbsClient::OnLbsMessageReceived(
    UniLbsConnection *c, base::Unpacker *p, uint16_t uri) {
  callback_->OnLbsMessageReceived(c, p, uri);
}

void UniLbsClient::OnJoinReplied(
    UniLbsConnection *c, const protocol::lbs::DetailInfo2Map &details,
    vector<ChannelInfo> *channels) {
  (void)details;
  callback_->OnFullChannelListRequired(c, channels);
}

void UniLbsClient::OnEdgeServerPerformanceRequired(
    uint32_t *user_count, uint32_t *cpu_percentage,
    uint32_t *rx_kbps, uint32_t *tx_kbps) {
  callback_->OnEdgeServerPerformanceRequired(
      user_count, cpu_percentage, rx_kbps, tx_kbps);
}

void UniLbsClient::OnUpdateEdgeServerLoadQuota(
     UniLbsConnection *c, uint32_t max_users) {
  (void)c;

  callback_->OnUpdateEdgeServerLoadQuota(max_users);
}

void UniLbsClient::ReloadLbsHosts() {
  vector<uint32_t> ips = network::ResolveHost(lbs_host_);
  if (ips.empty()) {
    ips.push_back(INADDR_LOOPBACK);
  }

  vector<NewSockAddress4> addrs;
  for (auto ip : ips) {
    addrs.emplace_back(ip, lbs_port_);
  }

  ReloadLbsAddresses(addrs);
}

void UniLbsClient::ReloadLbsAddresses(
    const vector<NewSockAddress4> &new_addresses) {
  unordered_map<uint32_t, UniLbsConnectionPtr> new_connections;
  for (const auto &addr : new_addresses) {
    uint32_t ip = addr.ip();
    auto conn_iter = connections_.find(ip);
    if (conn_iter != connections_.end()) {
      if (new_connections.find(ip) == new_connections.end()) {
        new_connections.insert(make_pair(ip, std::move(conn_iter->second)));
      }
    } else {
      UniLbsConnectionPtr conn(new UniLbsConnection(loop_, addr, service_name_,
                                                    service_id_, service_port_,
                                                    release_version_, this));
      conn->Connect();
      new_connections[ip] = std::move(conn);
    }
  }
  connections_.swap(new_connections);
}

}  // namespace lbs
}  // namespace agora
