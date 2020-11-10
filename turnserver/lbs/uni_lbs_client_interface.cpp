// Copyright (c) 2014-2017 Agora.io, Inc.
// A class helping to communnicating with a single LBS server,
//
//
#include "lbs/uni_lbs_client_interface.h"

#include <string>

#include "event2/event.h"
#include "lbs/uni_lbs_client.h"

namespace agora {
namespace lbs {

using std::string;
using std::vector;

class CallbackAdaptor : public UniLbsClient::Visitor {
 public :
  explicit CallbackAdaptor(UniLbsClientInterface::Callback *callback);
  virtual void OnLbsConnected(UniLbsConnection *c) OVERRIDE;

  virtual void OnLbsDisconnected(UniLbsConnection *c) OVERRIDE;

  // Received a message from |UniLbsConnection|
  virtual void OnLbsMessageReceived(UniLbsConnection *c, base::Unpacker *p,
                                    uint16_t uri) OVERRIDE;

  virtual void OnFullChannelListRequired(
      UniLbsConnection *c, std::vector<ChannelInfo> *channels) OVERRIDE;

  virtual void OnEdgeServerPerformanceRequired(
      uint32_t *users, uint32_t *cpu_percentage, uint32_t *rx_kbps,
      uint32_t *tx_kbps) OVERRIDE;

  virtual void OnUpdateEdgeServerLoadQuota(uint32_t max_users) OVERRIDE;
  virtual void OnAllLbsDisconnected() OVERRIDE;

 private:
  UniLbsClientInterface::Callback* callback_;
};

CallbackAdaptor::CallbackAdaptor(UniLbsClientInterface::Callback *callback) :
    callback_(callback) {
}

void CallbackAdaptor::OnLbsConnected(UniLbsConnection *c) {
  callback_->OnLbsConnected(c);
}

void CallbackAdaptor::OnLbsDisconnected(UniLbsConnection *c) {
  callback_->OnLbsDisconnected(c);
}

void CallbackAdaptor::OnLbsMessageReceived(UniLbsConnection *c,
                                           base::Unpacker *p,
                                           uint16_t uri) {
  callback_->OnLbsMessageReceived(c, p, uri);
}

void CallbackAdaptor::OnFullChannelListRequired(UniLbsConnection *c,
                                                vector<ChannelInfo> *channels) {
  callback_->OnFullChannelListRequired(c, channels);
}

void CallbackAdaptor::OnEdgeServerPerformanceRequired(
         uint32_t *users, uint32_t *cpu_percentage, uint32_t *rx_kbps,
         uint32_t *tx_kbps) {
  callback_->OnEdgeServerPerformanceRequired(users, cpu_percentage, rx_kbps,
                                             tx_kbps);
}

void CallbackAdaptor::OnUpdateEdgeServerLoadQuota(uint32_t max_users) {
  callback_->OnUpdateEdgeServerLoadQuota(max_users);
}

void CallbackAdaptor::OnAllLbsDisconnected() {
  callback_->OnAllLbsDisconnected();
}

UniLbsClientInterface::UniLbsClientInterface(
    const string& host, const string &service_name, uint16_t lbs_port,
    event_base *loop, Callback *callback) {
  auto adaptor = new CallbackAdaptor(callback);
  client_.reset(new UniLbsClient(host, service_name, lbs_port, "", loop,
                                 adaptor));
}

UniLbsClientInterface::~UniLbsClientInterface() {
}

void UniLbsClientInterface::SetReloadingInterval(uint32_t seconds) {
  client_->SetReloadingInterval(seconds);
}

void UniLbsClientInterface::SetServiceInformation(uint32_t service_id,
                                                  uint32_t service_port) {
  client_->SetServiceInformation(service_id, service_port);
}

void UniLbsClientInterface::DoChannelCreate(uint32_t vid, uint32_t cid,
                                            const string &name) {
  client_->DoChannelCreate(vid, cid, name);
}

void UniLbsClientInterface::DoChannelDestroy(uint32_t vid, uint32_t cid,
                                             const string& name) {
  client_->DoChannelDestroy(vid, cid, name);
}

}  // namespace lbs
}  // namespace agora
