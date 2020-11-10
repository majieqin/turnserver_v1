// Copyright (c) 2014-2017 Agora.io, Inc.
// Implementation of UniLbsConnection
//

#include "uni_lbs_connection.h"

#include <cassert>
#include <cstdint>
#include <exception>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "channel_type.h"

#include "error_code.h"
#include "packer.h"
#include "server_id.h"
#include "safe_log.h"
#include "uni_lbs_protocol.h"

namespace agora {
namespace lbs {

#define try          if(true)
#define catch(...)   if(false)

using std::make_pair;
using std::move;
using std::string;
using std::vector;

using base::kUniLbsType;
using base::RepeatedTimer;

UniLbsConnection::UniLbsConnection(
    event_base *loop,
    const network::NewSockAddress4 &lbs_addr,
    const string &service_name,
    uint32_t service_id, uint16_t service_port,
    const string &release_version, UniLbsEventCallback *callback)
    : connection_(loop, lbs_addr, this) {
  loop_ = loop;
  joined_ = false;

  service_name_ = service_name;
  service_id_ = service_id;
  service_port_ = service_port;

  release_version_ = release_version;
  callback_ = callback;
}

UniLbsConnection::~UniLbsConnection() {
  SAFE_LOG2(Info) << "Dis-Connecting to Uni Lbs: "
                  << connection_.GetRemoteAddress2().ToDebugString();
}

bool UniLbsConnection::Connect() {
  SAFE_LOG2(Info) << "Connecting to Uni Lbs: "
                  << connection_.GetRemoteAddress2().ToDebugString();
  return connection_.Connect();
}

bool UniLbsConnection::SendPacket(const base::Packet &p) {
  if (connection_.IsConnected()) {
    return connection_.SendPacket(p);
  }

  return false;
}

void UniLbsConnection::OnConnect(network::TcpClient *c, bool connected) {
  (void)c;
  assert(c == &connection_);

  if (!connected) {
    joined_ = false;

    callback_->OnLbsDisconnected(this);
    return;
  }

  callback_->OnLbsConnected(this);

  protocol::lbs::LbsJoinReq2 request(kUniLbsType);
  request.id = service_id_;
  request.service_name = service_name_;
  request.port = service_port_;
  request.detail[protocol::lbs::kServerVersion] = release_version_;
  connection_.SendPacket(request);
}

void UniLbsConnection::OnPacketReceived(
    network::TcpClient *c, base::Unpacker *p, uint16_t service_type,
    uint16_t uri) {

  if (service_type != kUniLbsType) {
    SAFE_LOG2(Warn) << "Not a unified lbs packet: packet ("
                    << service_type
                    << ", " << uri << ") from "
                    << c->GetRemoteAddress2().ToDebugString();
    return;
  }

  try {
    switch (uri) {
      case protocol::lbs::kLbsJoinResUri:
        OnJoinReplied(c, p);
        break;
      case protocol::lbs::kLbsJoinRes2Uri:
        OnJoin2Replied(c, p);
        break;
      case protocol::lbs::kLbsPongUri:
        OnPong(c, p);
        break;
      default:
        callback_->OnLbsMessageReceived(this, p, uri);
        break;
    }
  }
  catch (const std::exception &e) {
    SAFE_LOG2(Warn) << "Error in parsing packets (" << service_type
                    << ", " << uri << ") from "
                    << c->GetRemoteAddress2().ToDebugString();

    // Force to close the connection, and start again automatically
    c->Close();
  }
}

void UniLbsConnection::TimeToPing(network::TcpClient *conn) {
  (void)conn;
  assert(conn == &connection_);

  if (!joined_) return;

  uint32_t users = 0;
  uint32_t cpu = 0;
  uint32_t rx_kbps = 0;
  uint32_t tx_kbps = 0;

  callback_->OnEdgeServerPerformanceRequired(&users, &cpu, &rx_kbps, &tx_kbps);

  protocol::lbs::LbsPing ping(kUniLbsType);
  ping.id = service_id_;
  auto& load_detail = ping.load_detail;
  load_detail[protocol::lbs::kUserLoad] = users;
  load_detail[protocol::lbs::kCpuLoad] = cpu;
  load_detail[protocol::lbs::kTrafficInLoad] = rx_kbps;
  load_detail[protocol::lbs::kTrafficOutLoad] = tx_kbps;

  SendPacket(ping);
}

void UniLbsConnection::OnSocketError(network::TcpClient *conn) {
  SAFE_LOG2(Warn) << "socket to lbs "
                  << conn->GetRemoteAddress2().ToDebugString() << " is broken";

  joined_ = false;
}

void UniLbsConnection::OnJoinReplied(network::TcpClient *c, base::Unpacker *p) {
  (void)c;
  assert(c == &connection_);

  protocol::lbs::LbsJoinRes reply;
  reply.Unmarshall(p);

  SAFE_LOG2(Info) << "Reply from lbs "
                  << c->GetRemoteAddress2().ToDebugString()
                  << ", code: " << reply.code;

  if (reply.code == base::kOk) {
    joined_ = true;

    vector<ChannelInfo> channels;
    DetailInfo2Map dummy;
    callback_->OnJoinReplied(this, dummy, &channels);
    SendFullChannelList(move(channels));
  }
}

void UniLbsConnection::OnJoin2Replied(
    network::TcpClient *c, base::Unpacker *p) {
  (void)c;
  protocol::lbs::LbsJoinRes2 reply;
  reply.Unmarshall(p);
  if (reply.code == base::kOk) {
    joined_ = true;
    vector<ChannelInfo> channels;
    callback_->OnJoinReplied(this, reply.detail, &channels);
    SendFullChannelList(move(channels));
  }
}

void UniLbsConnection::OnPong(network::TcpClient *c, base::Unpacker *p) {
  (void)c;
  (void)p;

  protocol::lbs::LbsPong pong;
  pong.Unmarshall(p);
  auto load = pong.load_detail[protocol::lbs::kUserLoadMax];
  callback_->OnUpdateEdgeServerLoadQuota(this, load);

  LOG2(Debug, "lbs %s reply pong, max load: %d",
       c->GetRemoteAddress2().ToDebugString().c_str(), load);
}

void UniLbsConnection::SendFullChannelList(vector<ChannelInfo> channels) {
  protocol::lbs::LbsFullChannelList all(kUniLbsType);
  all.id = service_id_;

  for (auto it = channels.begin(); it != channels.end(); ++it) {
    all.channels.emplace_back(it->vid, move(it->channel_name));
  }

  SendPacket(all);
}

void UniLbsConnection::DoChannelCreate(ChannelInfo c) {
  if (!joined_) return;

  protocol::lbs::LbsDeltaChannelList delta(kUniLbsType);
  delta.id = service_id_;
  delta.added.emplace_back(c.vid, move(c.channel_name));

  SendPacket(delta);
}

void UniLbsConnection::DoChannelDestroy(ChannelInfo c) {
  if (!joined_) return;

  protocol::lbs::LbsDeltaChannelList delta(kUniLbsType);
  delta.id = service_id_;
  delta.removed.emplace_back(c.vid, move(c.channel_name));

  SendPacket(delta);
}

}  // namespace lbs
}  // namespace agora
