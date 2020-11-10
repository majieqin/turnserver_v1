// Copyright (c) 2014-2017 Agora.io, Inc.
// A class helping to communnicating with multiple voc services,
// for allocating a unique identifier.

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "network/address_types.h"
#include "base/cpp_feature.h"
#include "base/timer.h"

#include "network/voc_connection.h"

namespace agora {
namespace network {

struct VocEventCallback {
  // |ip| is host-ordered
  virtual void OnVocJoined(VocConnection *c, uint32_t code, uint32_t service_id,
                           uint32_t ip, const base::ConfigMap &config) = 0;

  virtual void OnServicesUpdated(
      const std::string &path, const std::vector<ServiceInfo> &in,
      const std::vector<ServiceInfo> &gone);

 protected:
  virtual ~VocEventCallback() {}
};

class VocAgent : private VocConnectionCallback, private base::NonCopyOrMovable {
 public:
  // |local_ip| should be host-ordered
  explicit VocAgent(event_base *loop, const std::string &path,
                    VocEventCallback *callback, uint32_t local_ip = 0,
                    bool use_local_ip = false);

  VocAgent(base::EventLoop *loop, const std::string &path,
           VocEventCallback *callback, uint32_t local_ip = 0,
           bool use_local_ip = false);

  virtual ~VocAgent();

  uint32_t my_id() const;
  uint32_t my_ip() const;
  const std::string& my_path() const;
  uint16_t service_port() const;

  void SetServicePort(uint16_t port);
  void Connect();

  void SubscribeService(const std::string &path);
  void UnsubscribeService(const std::string &path);

 private:
  typedef std::unique_ptr<VocConnection> VocConnectionPtr;
  typedef std::map<uint32_t, VocConnectionPtr> VocConnectionMap;

  virtual void OnSocketError(VocConnection *from) OVERRIDE;
  virtual void OnJoinReplied(
      VocConnection *from, uint32_t code, uint32_t service_id, uint32_t ip,
      const base::ConfigMap &config) OVERRIDE;

  virtual void OnAllServiceInfoReceived(
      VocConnection *from, const std::string &path,
      const ServiceInfoList &all) OVERRIDE;

  virtual void OnDeltaServiceInfoReceived(
      VocConnection *from, const std::string &path,
      const std::vector<ServiceInfo> &in,
      const std::vector<ServiceInfo> &gone) OVERRIDE;

  void TryNextVoc();
  void OnTimer();

  void MergeAllServicesInfo(const std::string &path);

 private:
  uint32_t my_id_;  // server ID assigned by voc.exe
  uint32_t my_ip_;  // near endpoint IP detected by voc.exe
  std::string my_path_;  // lable or category declared to voc.exe
  uint32_t local_ip_;
  uint16_t service_port_;  // udp port for other node to connect
  VocEventCallback *callback_;

  VocConnectionMap voc_list_;
  VocConnectionMap::const_iterator master_iter_;

  base::ConfigMap config_;
  InterestedServiceMap all_services_;

  base::RepeatedTimer timer_;
};

inline uint32_t VocAgent::my_id() const {
  return my_id_;
}

inline uint32_t VocAgent::my_ip() const {
  return my_ip_;
}

inline const std::string& VocAgent::my_path() const {
  return my_path_;
}

inline uint16_t VocAgent::service_port() const {
  return service_port_;
}

}  // namespace network
}  // namespace agora
