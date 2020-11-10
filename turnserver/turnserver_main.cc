/*
 *  Copyright 2012 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <iostream>  // NOLINT
#include <net/if.h>
#include <netinet/in.h>
#include <vector>
#include <thread>
#include <time.h>
#include <stdio.h>

#include "p2p/base/basicpacketsocketfactory.h"
#include "p2p/base/turnserver.h"
#include "rtc_base/asyncudpsocket.h"
#include "rtc_base/asynctcpsocket.h"
#include "rtc_base/asyncsocket.h"
#include "rtc_base/optionsfile.h"
#include "rtc_base/stream.h"
#include "rtc_base/stringencode.h"
#include "rtc_base/thread.h"
#include "rtc_base/sigslot.h"
//unilbs
#include "base/error_code.h"
#include "base/config.h"
#include "base/event_loop.h"
#include "base/server_id.h"
#include "base/unpacker.h"
#include "base/time_util.h"
#include "lbs/uni_lbs_client.h"
#include "lbs/uni_lbs_connection.h"
#include "protocol/uni_lbs_protocol.h"
#include "network/tcp_connection.h"
#include "network/tcp_server.h"
#include "base/system_info.h"
//elbs
#include "json_wrapper.h"
#include "new_elbs_protocol.h"
#include "thrift/gen-cpp/report_item_types.h"
#include "thrift/protocol/TCompactProtocol.h"
#include "thrift/transport/TBufferTransports.h"

using namespace agora;
using namespace agora::commons;
using namespace agora::base;
using namespace agora::commons::cjson;
using namespace rtc;
using namespace std;

using base::ErrorCode;
using base::EventLoop;
using base::ServiceType;
using base::Unpacker;
using lbs::ChannelInfo;
using lbs::UniLbsClient;
using lbs::UniLbsConnection;
using network::TcpConnection;
using network::TcpServer;
using protocol::lbs::ServiceUri;

struct UniLbsClientConstants {
	public:
		static constexpr const char *kLbsAddr = "42.81.206.84";
		static constexpr const uint16_t KLbsPort = 3100;
		static constexpr const char *kServiceName = "web_cloud_proxy_service";
		static constexpr const uint32_t kCientReloadingIntervalInSeconds = 10u;
};

struct WorkerConstants {
	public:
		static constexpr const char *kSoftware = "libjingle TurnServer";
		static constexpr const size_t kMaxPacketSize = 64 * 1024;
};

static constexpr const char* kTurnServerVer = "turnserverVersion1.1.0";
static constexpr const uint16_t kElbsPort = 9700;
static const uint16_t ARGUS_PING_LENGTH = 2;

typedef uint16_t PacketLength;

uint16_t udpport = 0;
uint16_t tcpport = 0;
int pid = 0;

enum {
	MSG_WORKER_ONTIMER,
	MSG_BANDWIDTH_GET,
	MSG_ELBS_ONTIMER,
	MSG_ARGUS_ONTIMER,
	MSG_LOG_ONTIMER,
};

class WorkerTCPSocket;
class ElbsConnection;
class Worker;

static std::vector<uint32_t> resolve_host(const std::string & host)
{
	std::vector<uint32_t> ips;

	hostent * he = ::gethostbyname(host.c_str());
	if (he == NULL) {
		std::cout << "resolve host " << host.c_str() << " failed " << h_errno << std::endl;
		return ips;
	}
	int i = 0;
	while (he->h_addr_list[i] != NULL)
	{
		ips.push_back(*(uint32_t*)he->h_addr_list[i]);
		++i;
	}
	return ips;
}

static std::string gettimestring() {
	time_t t;
	struct tm *nowtime;
	time(&t);
	nowtime = gmtime(&t);

	char timechar[256] = {0};
	sprintf(timechar, "%d-%02d-%02dT%02d:%02d:%02dZ: ", nowtime->tm_year+1900, nowtime->tm_mon+1, nowtime->tm_mday, nowtime->tm_hour, nowtime->tm_min, nowtime->tm_sec);
	std::string ret(timechar);
	return ret;
}

class TurnFileAuth : public cricket::TurnAuthInterface {
	public:
		explicit TurnFileAuth(const std::string& path) : file_(path) {
			file_.Load();
		}
		virtual bool GetKey(const std::string& username, const std::string& realm,
				std::string* key) {
			// File is stored as lines of <username>=<HA1>.
			// Generate HA1 via "echo -n "<username>:<realm>:<password>" | md5sum"
			std::string hex;
			bool ret = file_.GetStringValue(username, &hex);
			if (ret) {
				char buf[32];
				size_t len = rtc::hex_decode(buf, sizeof(buf), hex);
				*key = std::string(buf, len);
			}
			return ret;
		}
	private:
		rtc::OptionsFile file_;
};

class LogFile {
	public:
		LogFile(const std::string& path) : logfile_(path) {};
		~LogFile() {};

		size_t getFileSize() {
			FileStream log;
			int err;
			if (!log.Open(logfile_, "r", &err)) {
				std::cout << "Could not open file, err=" << err << std::endl;
				return 0;
			}
			size_t length = 0;
			if (!log.GetSize(&length)) {
				std::cout << "Could not get file size" << std::endl;
			}
			return length;
		}

		bool backupLogFile() {
			time_t t;
			struct tm *nowtime;
			time(&t);
			nowtime = gmtime(&t);
			size_t logsize = getFileSize();
			if(logsize < 1*1000*1000 || nowtime->tm_hour != 0) {
				return false;
			}
			char filename[256];
			sprintf(filename, "%s-%d-%02d-%02dT%02d:%02d:%02dZ", logfile_.c_str(), nowtime->tm_year+1900, nowtime->tm_mon+1, nowtime->tm_mday, nowtime->tm_hour, nowtime->tm_min, nowtime->tm_sec);
			std::cout << "backup log file: " << filename << std::endl;
			std::string backupfile(filename);
			copy_file(logfile_, backupfile);
			char cmd1[256] = {0};
			sprintf(cmd1, "cat /dev/null > %s", logfile_.c_str());
			system(cmd1);
			char cmd2[256] = {0};
			sprintf(cmd2, "gzip %s", filename);
			system(cmd2);
			return true;
		}

	private:
		void copy_file(std::string& source_path, std::string& destination_path) {
			char buffer[1024];
			FileStream in,out;
			int err;
			if (!in.Open(source_path, "r", &err)) {
				std::cout << "Could not open source_path file, err=" << err << std::endl;
				return;
			}
			if (!out.Open(destination_path, "w", &err)) {
				std::cout << "Could not open destination_path file, err=" << err << std::endl;
				return;
			}
			size_t inlen, outlen;
			while(in.Read(buffer, 1024, &inlen, &err) == SR_SUCCESS){
				out.Write(buffer, inlen, &outlen, &err);
				out.Flush();
			}
			size_t length = 0;
			out.GetSize(&length);
			std::cout << "backup log file size: " << length << std::endl;
		}

	private:
		std::string logfile_;
};

struct ArgusReportHead {
	uint16_t size_;
	uint8_t type_;
	uint8_t qos_;
	uint32_t seq_;
	uint32_t send_ts_;
	uint32_t vid_;
	uint32_t cid_;
};

enum class ReportItemType {
	Counter = 1000,
	Session = 1001,
	Vocs = 1002,
	Vos = 1003,
	WebQuit = 1029,
	WrtcConnectProxyTurnWorker = 2026,
};


class WorkerTCPSocket : public AsyncTCPSocketBase {
	public:
		// Binds and connects |socket| and creates AsyncTCPSocket for
		// it. Takes ownership of |socket|. Returns null if bind() or
		// connect() fail (|socket| is destroyed in that case).
		static WorkerTCPSocket* Create(rtc::Thread* thread,
				const SocketAddress& bind_address,
				const SocketAddress& remote_address) {
			rtc::AsyncSocket* tcp_client = thread->socketserver()->CreateAsyncSocket(bind_address.family(), SOCK_STREAM);
			rtc::AsyncSocket* client = AsyncTCPSocketBase::ConnectSocket(tcp_client, bind_address, remote_address);
			if(client == nullptr) return nullptr;
			return new WorkerTCPSocket(client, false);
		}
		WorkerTCPSocket(AsyncSocket* socket, bool listen) : AsyncTCPSocketBase(socket, listen, WorkerConstants::kMaxPacketSize){
			socket_.reset(socket);
		}
		~WorkerTCPSocket() override {}

		int Send(const void* pv,
				size_t cb,
				const rtc::PacketOptions& options) override {
			if (cb > WorkerConstants::kMaxPacketSize) {
				SetError(EMSGSIZE);
				return -1;
			}

			// If we are blocking on send, then silently drop this packet
			if (!IsOutBufferEmpty())
				return static_cast<int>(cb);

			//PacketLength pkt_len = HostToNetwork16(static_cast<PacketLength>(cb));
			//AppendToOutBuffer(&pkt_len, kPacketLenSize);
			AppendToOutBuffer(pv, cb);

			int res = FlushOutBuffer();
			if (res <= 0) {
				// drop packet if we made no progress
				ClearOutBuffer();
				return res;
			}

			rtc::SentPacket sent_packet(options.packet_id, rtc::TimeMillis());
			SignalSentPacket(this, sent_packet);

			// We claim to have sent the whole thing, even if we only sent partial
			return static_cast<int>(cb);
		}
		void ProcessInput(char* data, size_t* len) override {
			SocketAddress remote_addr(GetRemoteAddress());
			SignalReadPacket(this, data, (PacketLength)*len, remote_addr, CreatePacketTime(0));
			*len = 0;
			return;

			/*while (true) {
				if (*len < kPacketLenSize)
				return;

				PacketLength pkt_len = rtc::GetBE16(data);
				if (*len < kPacketLenSize + pkt_len)
				return;

				SignalReadPacket(this, data + kPacketLenSize, pkt_len, remote_addr,
				CreatePacketTime(0));

			 *len -= kPacketLenSize + pkt_len;
			 if (*len > 0) {
			 memmove(data, data + kPacketLenSize + pkt_len, *len);
			 }
			 }*/
		}
		void HandleIncomingConnection(AsyncSocket* socket) override {
			SignalNewConnection(this, new WorkerTCPSocket(socket, false));
		}

		bool ReConnect() {
			SocketAddress remote_addr(GetRemoteAddress());
			std::cout << "reconnect to " << remote_addr << std::endl;
			if (socket_->Connect(remote_addr) < 0) {
				std::cout << "Connect() to " << remote_addr << " failed with error " << socket_->GetError() << std::endl;
				return false;
			}
			return true;
		}

	private:
		std::unique_ptr<AsyncSocket> socket_;

		RTC_DISALLOW_COPY_AND_ASSIGN(WorkerTCPSocket);
};

void SendSessionReport(WorkerTCPSocket* socket) {
	if(socket == nullptr) return;
	std::cout << "start report!" << std::endl;
	ReportItem item;
	item.__set_uri((int)ReportItemType::Session);
	item.__isset.vosdkSession = true;
	VosdkSession& evt = item.vosdkSession;

	evt.__isset.header = true;
	evt.header.__set_sid("07EA2A894DDB45AC8695A5CF8DBCC8E4");
	evt.header.__set_lts(rtc::TimeMillis());
	//if (!ctx_->channelName().empty()) {
	evt.header.__set_cname("testtesttest");
	//}
	//if (ctx_->uid()) {
	evt.header.__set_uid(1);
	//}
	//if (ctx_->cid()) {
	evt.header.__set_cid(12345678);
	//}

	evt.__set_vk("67466468aed5442c977082bb2a43a1af");
	evt.__set_ver("2.6.0");
	evt.__set_cheVer("turnserver");
	evt.__set_localIp("127.0.0.1");
	evt.__set_net1(1);
	evt.__set_os1(7);
	evt.__set_did("test");
	evt.__set_pnq(-1);

	evt.__set_info("turnserver test report");

	std::cout << "vosdk session: " << evt << std::endl;

	apache::thrift::protocol::TCompactProtocolFactory protocolFactory;
	apache::thrift::transport::TTransportFactory transportFactory;
	auto trans = std::make_shared<apache::thrift::transport::TMemoryBuffer>();
	auto p = protocolFactory.getProtocol(transportFactory.getTransport(trans));
	if (item.write(p.get()) > 0) {
		p->getTransport()->flush();
		if (trans->available_read()) {
			uint8_t *p = nullptr;
			uint32_t size = 0;
			trans->getBuffer(&p, &size);
			std::cout << "p: " << p << " size: " << size << std::endl;
			if (p && size > 0) {
				ArgusReportHead head;
				std::vector<char> buffer;
				buffer.resize(size + sizeof(head));

				head.size_ = buffer.size();
				head.type_ = 3;
				head.qos_ = 0;
				head.seq_ = 0;
				head.send_ts_ = rtc::TimeMillis() / 1000;
				head.vid_ = 161891;
				head.cid_ = 12345678;

				memcpy(&buffer[0], &head, sizeof(head));
				if (size > 0) {
					memcpy(&buffer[sizeof(head)], p, size);  // payload
				}
				rtc::PacketOptions options;
				int ret = socket->Send(buffer.data(), buffer.size(), options);
				std::cout << "send session report return " << ret << std::endl;
			}
		}
	}

}

void SendQuitReport(WorkerTCPSocket* socket) {
	if(socket == nullptr) return;
	std::cout << "start quit report!" << std::endl;
	ReportItem item;
	item.__set_uri((int)ReportItemType::WebQuit);
	item.__isset.vosdkWebQuit = true;
	VosdkWebQuit& evt = item.vosdkWebQuit;

	evt.__isset.header = true;
	evt.header.__set_sid("697abf9b17da4d5a8a4df0fe0ea4b531");
	evt.header.__set_lts(rtc::TimeMillis());
	//if (!ctx_->channelName().empty()) {
	evt.header.__set_cname("machine");
	//}
	//if (ctx_->uid()) {
	evt.header.__set_uid(124487);
	//}
	//if (ctx_->cid()) {
	evt.header.__set_cid(2888778408);
	//}

	evt.__set_ec(0);
	evt.header.__set_elapse(1000);

	apache::thrift::protocol::TCompactProtocolFactory protocolFactory;
	apache::thrift::transport::TTransportFactory transportFactory;
	auto trans = std::make_shared<apache::thrift::transport::TMemoryBuffer>();
	auto p = protocolFactory.getProtocol(transportFactory.getTransport(trans));
	if (item.write(p.get()) > 0) {
		p->getTransport()->flush();
		if (trans->available_read()) {
			uint8_t *p = nullptr;
			uint32_t size = 0;
			trans->getBuffer(&p, &size);
			std::cout << "p: " << p << " size: " << size << std::endl;
			if (p && size > 0) {
				ArgusReportHead head;
				std::vector<char> buffer;
				buffer.resize(size + sizeof(head));

				head.size_ = buffer.size();
				head.type_ = 3;
				head.qos_ = 0;
				head.seq_ = 0;
				head.send_ts_ = rtc::TimeMillis() / 1000;
				head.vid_ = 161891;
				head.cid_ = 2888778408;

				memcpy(&buffer[0], &head, sizeof(head));
				if (size > 0) {
					memcpy(&buffer[sizeof(head)], p, size);  // payload
				}
				rtc::PacketOptions options;
				int ret = socket->Send(buffer.data(), buffer.size(), options);
				std::cout << "send quit report return " << ret << std::endl;
			}
		}
	}

}

void SendConnectReport(WorkerTCPSocket* socket, std::string& clientaddr, int usercount, int ingress, int egress) {
	if(socket == nullptr) return;
	std::cout << "start quit report!" << std::endl;
	ReportItem item;
	item.__set_uri((int)ReportItemType::WrtcConnectProxyTurnWorker);
	item.__isset.wrtcConnectProxyTurnWorker = true;
	WrtcConnectProxyTurnWorker& evt = item.wrtcConnectProxyTurnWorker;

	evt.__isset.header = true;
	evt.header.__set_sid("697abf9b17da4d5a8a4df0fe0ea4b531");
	evt.header.__set_lts(rtc::TimeMillis());
	//if (!ctx_->channelName().empty()) {
	evt.header.__set_cname("machine");
	//}
	//if (ctx_->uid()) {
	evt.header.__set_uid(124487);
	//}
	//if (ctx_->cid()) {
	evt.header.__set_cid(2888778408);
	//}

	evt.__set_clientAddr(clientaddr);
	evt.__set_userCount(usercount);
	evt.__set_ingress(ingress);
	evt.__set_egress(egress);

	apache::thrift::protocol::TCompactProtocolFactory protocolFactory;
	apache::thrift::transport::TTransportFactory transportFactory;
	auto trans = std::make_shared<apache::thrift::transport::TMemoryBuffer>();
	auto p = protocolFactory.getProtocol(transportFactory.getTransport(trans));
	if (item.write(p.get()) > 0) {
		p->getTransport()->flush();
		if (trans->available_read()) {
			uint8_t *p = nullptr;
			uint32_t size = 0;
			trans->getBuffer(&p, &size);
			std::cout << "p: " << p << " size: " << size << std::endl;
			if (p && size > 0) {
				ArgusReportHead head;
				std::vector<char> buffer;
				buffer.resize(size + sizeof(head));

				head.size_ = buffer.size();
				head.type_ = 3;
				head.qos_ = 0;
				head.seq_ = 0;
				head.send_ts_ = rtc::TimeMillis() / 1000;
				head.vid_ = 161891;
				head.cid_ = 2888778408;

				memcpy(&buffer[0], &head, sizeof(head));
				if (size > 0) {
					memcpy(&buffer[sizeof(head)], p, size);  // payload
				}
				rtc::PacketOptions options;
				int ret = socket->Send(buffer.data(), buffer.size(), options);
				std::cout << "send connect report return " << ret << std::endl;
			}
		}
	}

}


class ElbsConnection : public rtc::MessageHandler, public sigslot::has_slots<> {
	public:
		ElbsConnection(rtc::Thread* thread) { thread_ = thread; socket_ = NULL; last_connect_ts_ = 0; }
		~ElbsConnection() { 
			if(socket_) {
				socket_->Close();
				socket_ = NULL;
			}
			if(thread_) {
				thread_->Clear(this, MSG_ELBS_ONTIMER);
				thread_->Clear(this, MSG_ARGUS_ONTIMER);
				thread_->Stop(); 
			}
			elbs_socket_.clear();
		}

		void start() {
			pingPkg_.resize(ARGUS_PING_LENGTH);
			*reinterpret_cast<uint16_t*>(&pingPkg_[0]) = ARGUS_PING_LENGTH;
			thread_->PostDelayed(RTC_FROM_HERE, 60 * 1000, this, MSG_ELBS_ONTIMER);
			thread_->PostDelayed(RTC_FROM_HERE, 3 * 1000, this, MSG_ARGUS_ONTIMER);
			joinElbs();
		}

		WorkerTCPSocket* getArgusSocket() { return socket_; }

	private:
		typedef std::vector<WorkerTCPSocket*> TcpClientList;

	private:
		void OnResponsePacket(rtc::AsyncPacketSocket* socket, const char* data,
				size_t size, const rtc::SocketAddress& address,
				const rtc::PacketTime& packet_time) {
			//std::cout << "HandleResponseMessage: recv message  " << data << " length " << size << std::endl;
			if(size < 4) return;
			Unpacker unpkr(data, size, false);
			uint16_t service_type = unpkr.PopUint16();
			uint16_t uri = unpkr.PopUint16();
			unpkr.Rewind();
			//std::string jcstr = unpkr.PopString();
			std::cout << "elbs recv s " << service_type << " u " << uri << " from:" << socket->GetRemoteAddress() << std::endl;

			switch (uri) {
				case ElbsUriType::kRejoinResponseUri: {
																								RejoinResponseProtocol resp;
																								resp.Unmarshall(&unpkr);

																								//std::cout << "response ip:" << resp.ip << " port:" << resp.port << std::endl;
																								int64_t now = rtc::TimeMillis();
																								if(last_connect_ts_ + 3000 > now) {
																									socket->Close();
																									return;
																								}
																								if(NetworkToHost32(resp.ip) == connect_Addr_.ip() && resp.port == connect_Addr_.port()) return;
																								connect_Addr_.SetIP(NetworkToHost32(resp.ip));
																								connect_Addr_.SetPort(resp.port);
																								std::cout << "edge server address:" << connect_Addr_ << std::endl;
																								if(connectEdgeServer(connect_Addr_)) {
																									last_connect_ts_ = now;
																								}
																								socket->Close();
																								break;
																							}
				default:
																							std::cout << "unexpected packet" << std::endl;
																							break;
			}

		}

		void OnElbsSocketConnect(rtc::AsyncPacketSocket* socket) {
			const rtc::SocketAddress& remote_address = socket->GetRemoteAddress();
			std::cout << "elbs connect to " << remote_address << " successfully!" << std::endl;
			RejoinRequestProtocol request(ELBS_SERVER_TYPE);
			request.ip = connect_Addr_.ip();
			request.port = connect_Addr_.port();
			Packer pk;
			pk << request;
			pk.Pack();
			rtc::PacketOptions options;
			socket->Send(pk.Buffer(), pk.Length(), options);
			//std::cout << "send elbs request return " << ret << std::endl;
		}

		void OnElbsSocketClose(rtc::AsyncPacketSocket* socket, int error) {
			std::cout << "Close from elbs server, error=" << error << std::endl;
			socket->Close();
		}

		void OnArgusSocketConnect(rtc::AsyncPacketSocket* socket) {
			const rtc::SocketAddress& remote_address = socket->GetRemoteAddress();
			std::cout << "edge server connect to " << remote_address << " successfully!" << std::endl;
			if(socket->GetLocalAddress() == socket_connecting_->GetLocalAddress()) {
				if(socket_) socket_->Close();
				socket_ = socket_connecting_;
				//SendSessionReport(socket_);
				SendQuitReport(socket_);
				std::string addr = "127.0.0.1:1234";
				SendConnectReport(socket_, addr, 0, 0, 0);
			} else {
				socket->Close();
			}
		}

		void OnArgusSocketClose(rtc::AsyncPacketSocket* socket, int error) {
			std::cout << "Close from edge server " << socket->GetRemoteAddress() << ", error=" << error << std::endl;
			socket->Close();
		}

		virtual void OnMessage(rtc::Message* msg) {
			//RTC_DCHECK(msg->message_id == MSG_ELBS_ONTIMER);
			switch(msg->message_id) {
				case MSG_ELBS_ONTIMER:
					{
						joinElbs();
						thread_->Clear(this, MSG_ELBS_ONTIMER);
						thread_->PostDelayed(RTC_FROM_HERE, 60 * 1000, this, MSG_ELBS_ONTIMER);

						break;
					}
				case MSG_ARGUS_ONTIMER:
					{
						if(socket_ && socket_->GetState() == rtc::AsyncTCPSocket::STATE_CONNECTED) {
							rtc::PacketOptions options;
							socket_->Send(pingPkg_.data(), pingPkg_.size(), options);
							//std::cout << "send edge server ping return " << ret << std::endl;
						}
						thread_->Clear(this, MSG_ARGUS_ONTIMER);
						thread_->PostDelayed(RTC_FROM_HERE, 3 * 1000, this, MSG_ARGUS_ONTIMER);
					}
			}

		}

		void joinElbs() {
			elbs_socket_.clear();
			std::vector<uint32_t> elbs_ips = resolve_host("qoslbs.agoralab.co");
			for (auto ip : elbs_ips) {
				rtc::SocketAddress elbsAddr(NetworkToHost32(ip), kElbsPort);
				std::cout << elbsAddr << std::endl;
				connectElbs(elbsAddr);
			}
		}

		void connectElbs(rtc::SocketAddress& serverAddr) {
			rtc::SocketAddress clientAddr(INADDR_ANY, 0);
			WorkerTCPSocket* socket = WorkerTCPSocket::Create(thread_, clientAddr, serverAddr);
			if(socket == nullptr) return;
			socket->SignalReadPacket.connect(this, &ElbsConnection::OnResponsePacket);
			socket->SignalConnect.connect(this, &ElbsConnection::OnElbsSocketConnect);
			socket->SignalClose.connect(this, &ElbsConnection::OnElbsSocketClose);
			//std::cout << "local address " << socket->GetLocalAddress() << " remote address " << socket->GetRemoteAddress() << std::endl;
			elbs_socket_.push_back(socket);
		}

		bool connectEdgeServer(rtc::SocketAddress& serverAddr) {
			rtc::SocketAddress clientAddr(INADDR_ANY, 0);
			WorkerTCPSocket* socket = WorkerTCPSocket::Create(thread_, clientAddr, serverAddr);
			if(socket == nullptr) return false;
			socket->SignalReadPacket.connect(this, &ElbsConnection::OnResponsePacket);
			socket->SignalConnect.connect(this, &ElbsConnection::OnArgusSocketConnect);
			socket->SignalClose.connect(this, &ElbsConnection::OnArgusSocketClose);
			//std::cout << "local address " << socket->GetLocalAddress() << " remote address " << socket->GetRemoteAddress() << std::endl;
			socket_connecting_ = socket;
			return true;
		}

	private:
		rtc::Thread* thread_;
		TcpClientList elbs_socket_;
		WorkerTCPSocket* socket_connecting_;
		WorkerTCPSocket* socket_;
		rtc::SocketAddress connect_Addr_;
		int64_t last_connect_ts_;
		std::vector<char> pingPkg_;
};

class Worker : public rtc::MessageHandler, public sigslot::has_slots<> {
	public:
		Worker(rtc::Thread* thread, cricket::TurnServer* turn, ElbsConnection* elbs) : log_file_("log/turnserver.log") { thread_ = thread; turn_ = turn; elbs_ = elbs; socket_ = NULL; }
		~Worker() { 
			if(socket_) {
				socket_->Close();
				socket_ = NULL;
			}
			if(thread_) {
				thread_->Clear(this, MSG_WORKER_ONTIMER);
				thread_->Clear(this, MSG_BANDWIDTH_GET);
				thread_->Clear(this, MSG_LOG_ONTIMER);
				thread_->Stop(); 
			}
		}

		// Starts listening for packets from internal clients.
		void AddWorkerManagerSocket(rtc::SocketAddress loacladdr, rtc::SocketAddress remoteaddr) {
			local_address = loacladdr;
			remote_address = remoteaddr;
			std::cout << gettimestring() << "local address " << local_address << " remote address " << remote_address << std::endl;
			WorkerTCPSocket* socket = WorkerTCPSocket::Create(thread_, local_address, remote_address);
			if(socket != nullptr) {
				socket_ = socket;
				socket->SignalReadPacket.connect(this, &Worker::OnWorkerManagerPacket);
				socket->SignalConnect.connect(this, &Worker::OnSocketConnect);
				socket->SignalClose.connect(this, &Worker::OnSocketClose);     
			}

			thread_->PostDelayed(RTC_FROM_HERE, 3000, this, MSG_WORKER_ONTIMER);
			thread_->PostDelayed(RTC_FROM_HERE, 60 * 1000, this, MSG_LOG_ONTIMER);
			thread_->PostDelayed(RTC_FROM_HERE, 500, this, MSG_BANDWIDTH_GET);
		}

		int sendConnectMsg() {
			// std::cout << "socket state " << socket_->GetState() << std::endl;
			JsonWrapper json;
			json.setObjectType();
			json.setStringValue("command", "connect");
			json.setStringValue("name", "webrtc_proxy");
			json.setIntValue("pid", pid);
			json.setDoubleValue("ts", rtc::TimeMillis());
			json.setStringValue("appId", "");
			json.setStringValue("cname", "");
			json.setStringValue("uid", "");
			json.setStringValue("minSdkVersion", "2.3.1");
			json.setStringValue("maxSdkVersion", "2.6.2");

			std::string jcstr;
			jcstr = json.toString();
			std::cout << "connect json: [" << jcstr << "] length: " << jcstr.length() << std::endl;
			WorkerManagerComm comm(UAP_SERVER_TYPE);
			comm.json = std::move(jcstr);
			Packer pk;
			pk << comm;
			pk.Pack();

			//char* buff = (char*)pk.Buffer();
			//std::cout<< *buff<<","<<*(buff+1)<<","<<*(buff+2)<<","<<*(buff+3)<<","<<*(buff+4)<<","<<*(buff+5)<<","<<*(buff+6)<<","<<*(buff+7)<<","<<*(buff+8)<<","<<*(buff+9)<<","<<*(buff+10)<<","<<*(buff+11)<<std::endl;
			//std::cout<< (unsigned int)(unsigned char)(*buff)<<","<<(unsigned int)(unsigned char)*(buff+1)<<","<<(unsigned int)(unsigned char)*(buff+2)<<","<<(unsigned int)(unsigned char)*(buff+3)<<","<<(unsigned int)(unsigned char)*(buff+4)<<","<<(unsigned int)(unsigned char)*(buff+5)<<","<<(unsigned int)(unsigned char)*(buff+6)<<","<<(unsigned int)(unsigned char)*(buff+7)<<","<<(unsigned int)(unsigned char)*(buff+8)<<","<<(unsigned int)(unsigned char)*(buff+9)<<","<<(unsigned int)(unsigned char)*(buff+10)<<","<<(unsigned int)(unsigned char)*(buff+11) << std::endl;
			//std::cout <<std::hex<<std::setfill('0') << std::setw(2)<<(unsigned int)(unsigned char)(*buff) <<","<<(unsigned int)(unsigned char)*(buff+1)<<","<<(unsigned int)(unsigned char)*(buff+2)<<","<<(unsigned int)(unsigned char)*(buff+3)<<","<<(unsigned int)(unsigned char)*(buff+4)<<","<<(unsigned int)(unsigned char)*(buff+5)<<","<<(unsigned int)(unsigned char)*(buff+6)<<","<<(unsigned int)(unsigned char)*(buff+7)<<","<<(unsigned int)(unsigned char)*(buff+8)<<","<<(unsigned int)(unsigned char)*(buff+9)<<","<<(unsigned int)(unsigned char)*(buff+10)<<","<<(unsigned int)(unsigned char)*(buff+11) << std::endl;
			//std::cout << "connect packer: [" << pk.Buffer() << "] length: " << pk.Length() << std::endl;
			int ret = sendMessage(pk.Buffer(), pk.Length());
			//std::cout << "send return " << ret << std::endl;

			return ret;
		}

		int sendPingMsg() {
			JsonWrapper json;
			json.setObjectType();
			json.setStringValue("command", "ping");
			//if(userCount == 0 || ingress == 0) {
			json.setStringValue("state", "idle");
			//} else {
			// json.setStringValue("state", "active");
			//}
			json.setDoubleValue("ts", rtc::TimeMillis());
			JsonWrapper msg;
			msg.setObjectType();
			msg.setIntValue("cpu", 0);
			msg.setIntValue("ingress", ingress);
			msg.setIntValue("egress", egress);
			msg.setIntValue("userCount", userCount);
			json.setObjectValue("workload", msg);

			std::string jcstr;
			jcstr = json.toString();
			if(userCount > 0 || ingress > 0) {
				std::cout << "ping json: [" << jcstr << "] length: " << jcstr.length() << std::endl;
			}
			WorkerManagerComm comm(UAP_SERVER_TYPE);
			comm.json = std::move(jcstr);
			Packer pk;
			pk << comm;
			pk.Pack();
			int ret = sendMessage(pk.Buffer(), pk.Length());
			//std::cout << "send return " << ret << std::endl;
			return ret;
		}

	private:
		void OnWorkerManagerPacket(rtc::AsyncPacketSocket* socket, const char* data,
				size_t size, const rtc::SocketAddress& address,
				const rtc::PacketTime& packet_time) {
			//std::cout << "HandleWorkerManagerMessage: recv message  " << data << " length " << size << std::endl;
			Unpacker unpkr(data, size, false);
			uint16_t service_type = unpkr.PopUint16();
			uint16_t uri = unpkr.PopUint16();
			//unpkr.Rewind();
			std::string jcstr = unpkr.PopString();
			std::cout << gettimestring() << "worker recv s " << service_type << " u " << uri << " json: [" << jcstr << "] length: " << jcstr.length() << std::endl;
			JsonWrapper json(jcstr.c_str());
			std::string command;
			if(!json.tryGetStringValue("command", command)) {
				std::cout << "error to find command from json!" << std::endl;
				return;
			}

			if(command.compare("request") == 0) {
				std::string appid;
				std::string cname;
				std::string uid;
				std::string sid;
				std::string clientAddr;
				int seq, requestid;
				json.tryGetStringValue("appId", appid);
				json.tryGetStringValue("cname", cname);
				json.tryGetStringValue("uid", uid);
				json.tryGetStringValue("sid", sid);
				json.tryGetIntValue("seq", seq);
				json.tryGetIntValue("requestId", requestid);
				json.tryGetStringValue("sdkAddress", clientAddr);

				JsonWrapper resp;
				resp.setObjectType();
				resp.setStringValue("command", "response");
				resp.setStringValue("appId", appid);
				resp.setStringValue("cname", cname);
				resp.setStringValue("uid", uid);
				resp.setStringValue("sid", sid);
				resp.setIntValue("seq", seq);
				resp.setDoubleValue("ts", rtc::TimeMillis());
				resp.setIntValue("requestId", requestid);
				resp.setIntValue("code", 200);
				resp.setStringValue("reason", "Success");
				JsonWrapper msg;
				msg.setObjectType();
				msg.setIntValue("udpport", udpport);
				msg.setIntValue("tcpport", tcpport);
				msg.setStringValue("username", "test");
				msg.setStringValue("password", "111111");
				resp.setObjectValue("serverResponse", msg);


				std::string jcstr1;
				jcstr1 = resp.toString();
				std::cout << "response json: [" << jcstr1 << "] length: " << jcstr1.length() << std::endl;
				WorkerManagerComm comm(UAP_SERVER_TYPE);
				comm.json = std::move(jcstr1);
				Packer pk;
				pk << comm;
				pk.Pack();
				int ret = sendMessage(pk.Buffer(), pk.Length());
				std::cout << "send return " << ret << std::endl;

				//SendConnectReport(elbs_->getArgusSocket(), clientAddr, userCount, ingress, egress);
			}
		}

		void OnSocketConnect(rtc::AsyncPacketSocket* socket) {
			//const rtc::SocketAddress& remote_address = socket->GetRemoteAddress();
			std::cout << "worker connect to " << remote_address << " successfully!" << std::endl;
			sendConnectMsg();
		}

		void OnSocketClose(rtc::AsyncPacketSocket* socket, int error) {
			std::cout << gettimestring() << "Close from server, error=" << error << std::endl;
			RTC_DCHECK(socket == socket_);
			socket->Close();
		}

		int sendMessage(const void *pv, size_t cb) {
			if(socket_ == nullptr) return 0;
			if(socket_->GetState() != rtc::AsyncTCPSocket::STATE_CONNECTED) return 0;
			rtc::PacketOptions options;
			int ret = socket_->Send(pv, cb, options);
			return ret;
		}

		virtual void OnMessage(rtc::Message* msg) {
			//RTC_DCHECK(msg->message_id == 1);
			switch(msg->message_id) {
				case MSG_WORKER_ONTIMER:
					{
						if(socket_ == nullptr) {
							WorkerTCPSocket* socket = WorkerTCPSocket::Create(thread_, local_address, remote_address);
							if(socket != nullptr) {
								socket_ = socket;
								std::cout << "create new socket!" << std::endl;
								socket_->SignalReadPacket.connect(this, &Worker::OnWorkerManagerPacket);
								socket_->SignalConnect.connect(this, &Worker::OnSocketConnect);
								socket_->SignalClose.connect(this, &Worker::OnSocketClose);
							}         
						} else {
							rtc::AsyncTCPSocket::State state = socket_->GetState();
							// std::cout << "socket state " << state << std::endl;
							if(state == rtc::AsyncTCPSocket::STATE_CONNECTED) {
								sendPingMsg();
							} else if(state == rtc::AsyncTCPSocket::STATE_CLOSED) {
								WorkerTCPSocket* socket = WorkerTCPSocket::Create(thread_, local_address, remote_address);
								if(socket != nullptr) {
									socket_ = socket;
									std::cout << "create new socket!" << std::endl;
									socket_->SignalReadPacket.connect(this, &Worker::OnWorkerManagerPacket);
									socket_->SignalConnect.connect(this, &Worker::OnSocketConnect);
									socket_->SignalClose.connect(this, &Worker::OnSocketClose);
								}           
							} else {
								if(socket_->ReConnect()) {
									sendPingMsg();
								}
							}
						}
						thread_->Clear(this, MSG_WORKER_ONTIMER);
						thread_->PostDelayed(RTC_FROM_HERE, 3000, this, MSG_WORKER_ONTIMER);

						break;
					}
				case MSG_BANDWIDTH_GET:
					{
						userCount = turn_->getAllocationCount() / 2;
						//userCount = turn_->getChannelCount();
						ingress = turn_->getSendBandWidth();
						egress = turn_->getRecvBandWidth();
						std::cout << gettimestring() <<  " userCount: " << userCount << ", ingress: " << ingress << ", egress: " << egress << std::endl;
						thread_->Clear(this, MSG_BANDWIDTH_GET);
						thread_->PostDelayed(RTC_FROM_HERE, 500, this, MSG_BANDWIDTH_GET);

						break;
					}
				case MSG_LOG_ONTIMER:
					{
						//size_t length = log_file_.getFileSize();
						//std::cout << "log file size: " << length << std::endl;
						// log_file_.backupLogFile();
						thread_->Clear(this, MSG_LOG_ONTIMER);
						thread_->PostDelayed(RTC_FROM_HERE, 60 * 1000, this, MSG_LOG_ONTIMER);
						break;
					}
			}

		}

	private:
		rtc::Thread* thread_;
		cricket::TurnServer* turn_;
		WorkerTCPSocket* socket_;
		ElbsConnection* elbs_;
		rtc::SocketAddress local_address;
		rtc::SocketAddress remote_address;
		int userCount;
		int ingress;
		int egress;
		LogFile log_file_;
};

class UniLbsWorker: private lbs::UniLbsClient::Visitor {
	public:
		UniLbsWorker(base::EventLoop *n, cricket::TurnServer* turn, uint16_t port)
			: log_file_("log/turnserver.log"),
			port_(port),
			turn_(turn),
			network_loop_(n),
			system_info_(n->GetEventBase()) {
			}

		~UniLbsWorker(){
		}

		void Initialize() {

		}

		int AddToUniLbs() {
			std::cout << gettimestring() <<"Worker AddToUniLbs!!" <<std::endl;
			auto eventbase = network_loop_->GetEventBase();
			uint32_t service_id = base::TickInMilliSeconds(); 
			uni_lbs_client_.reset(new lbs::UniLbsClient("", 
						UniLbsClientConstants::kServiceName,
						base::kLbsEdgeServicePort,
						kTurnServerVer,
						eventbase,
						this));
			network::NewSockAddress4 lbs_addr;
			in_addr ip;
			if (!inet_aton(UniLbsClientConstants::kLbsAddr, &ip)) {
				return false;
			}
			lbs_addr.SetIp(ntohl(ip.s_addr));
			lbs_addr.SetPort(static_cast<uint16_t>(base::kLbsEdgeServicePort));
			vecAddresses.push_back(lbs_addr);

			// Step 1: connect to uni_lbs
			uni_lbs_client_->SetServiceInformation(service_id, port_);

			// Step 2: It is time to update the hosts safely
			uni_lbs_client_->UpdateLbsAddresses(vecAddresses);
			return 0;
		}

		void Run() {
			AddToUniLbs();
			network_thread_ = thread(WorkRun, this);
			test_thread_ = thread(WorkTest, this);
		}

	private:
		static void WorkRun(UniLbsWorker *worker) {
			std::cout << gettimestring() << "Worker unilibs WorkRun!!" <<std::endl;
			worker->network_loop_->Run();
		}

		static void WorkTest(UniLbsWorker *worker) {
			std::cout << gettimestring() << "Worker test WorkRun!!" <<std::endl;
			while(1) {
				worker->SystemInfoPrint();
				sleep(2);
			}
		}

		void SystemInfoPrint(){
			uint32_t user_count = turn_->getAllocationCount();
			uint32_t user_channel_count = turn_->getChannelCount();
			uint32_t cpu_percentage = static_cast<uint32_t>(system_info_.GetSelfCpuUsage());
			uint32_t rx_kbps;
			uint32_t tx_kbps;
			system_info_.GetNicFlowRate(&rx_kbps, &tx_kbps);
			std::cout << gettimestring() << "user_allocation count:"<< user_count  << " user_channel_count:" << user_channel_count  << " cpu_percentage:" << cpu_percentage << " rx_kbps:"<< rx_kbps << " tx_kbps:" << tx_kbps<<std::endl;
		}

		void OnChannelCreated(
				uint32_t vid, uint32_t cid, const string &name) {
			uni_lbs_client_->DoChannelCreate(vid, cid, name);
		}

		void OnChannelDestroyed(
				uint32_t vid, uint32_t cid, const string &name) {
			uni_lbs_client_->DoChannelDestroy(vid, cid, name);
		}

		// Inherited from |UniLbsClientVisitorCallback|
		void OnEdgeServerPerformanceRequired(
				uint32_t *user_count, uint32_t *cpu_percentage,
				uint32_t *rx_kbps, uint32_t *tx_kbps) {
			*user_count = turn_->getChannelCount();
			*cpu_percentage = static_cast<uint32_t>(system_info_.GetSelfCpuUsage());
			*tx_kbps = turn_->getSendBandWidth();
			*rx_kbps = turn_->getRecvBandWidth();
			// system_info_.GetNicFlowRate(rx_kbps, tx_kbps);
			std::cout << "EdgeServerPerformance-> user_count:"<<*user_count << " cpu_percentage:" << *cpu_percentage << " rx_kbps:"<<*rx_kbps << " tx_kbps:" <<*tx_kbps<<std::endl;
			log_file_.backupLogFile();
		};

		void OnFullChannelListRequired(
				UniLbsConnection *c, std::vector<ChannelInfo> *channels) {
			(void)c;
			(void)channels;
		};

		void OnLbsConnected(UniLbsConnection *c){ (void)c; };

		void OnLbsDisconnected(UniLbsConnection *c) { (void)c; };

		void OnAllLbsDisconnected(){};

		// Received a message from |UniLbsConnection|
		void OnLbsMessageReceived(UniLbsConnection *c, base::Unpacker *p,
				uint16_t uri) {
			(void)c;
			(void)p;
			(void)uri;
		};

		void OnUpdateEdgeServerLoadQuota(uint32_t max_users) { 
			(void)max_users;
		};
	private:
		LogFile log_file_;
		uint16_t port_;
		cricket::TurnServer* turn_;
		base::EventLoop *network_loop_;
		std::thread network_thread_;
		std::thread test_thread_;
		base::SystemInfo system_info_;
		std::vector<network::NewSockAddress4> vecAddresses;
		std::unique_ptr<lbs::UniLbsClient> uni_lbs_client_;
};

inline std::string GetExternIP() {
	std::string extIp;
	std::string cmd = "curl cip.cc";
	FILE *fp = popen(cmd.c_str(), "r");
	char buffer[256] = {0};
	while (NULL != fgets(buffer, 256, fp)) {
		std::string buf(buffer);
		std::cout << buf << std::endl;
		size_t pos = buf.find("IP");
		if (pos != std::string::npos) {
			std::string remain = buf.substr(pos + 2);
			if ((pos = remain.find_last_of(" ")) != std::string::npos) {
				std::string ipstr = remain.substr(pos + 1);
				std::cout << ipstr << std::endl;
				if ((pos = ipstr.find("\n")) != std::string::npos) {
					extIp = ipstr.substr(0, pos);
				} else {
					extIp = ipstr;
				}
			}
			break;
		}
	}
	return extIp;
}

void Foo(int i) {
	std::cout << __func__ << " called: " << i << std::endl;
}

#define BUILD_TIME __DATE__ " " __TIME__

int main(int argc, char **argv) {
	if (argc > 1 && !strcmp(argv[1], "-v")) {
		cout << "  version " << kTurnServerVer << " @ " << BUILD_TIME << endl;
		_Exit(0);
	}

	if (argc < 6) {
		std::cerr << "usage: turnserver int-udp-addr int-tcp-addr ext-ip realm auth-file"
			<< std::endl;
		return 1;
	}

	rtc::SocketAddress int_addr;
	if (!int_addr.FromString(argv[1])) {
		std::cerr << "Unable to parse IP address: " << argv[1] << std::endl;
		return 1;
	}

	udpport = int_addr.port();

	rtc::SocketAddress int_tcp_addr;
	if (!int_tcp_addr.FromString(argv[2])) {
		std::cerr << "Unable to parse IP address: " << argv[2] << std::endl;
		return 1;
	}

	tcpport = int_tcp_addr.port();

	std::string extaddr_str;
	extaddr_str = GetExternIP();
	std::cout << gettimestring() << "get extern address success: " << extaddr_str << std::endl;
	rtc::IPAddress ext_addr;
	if (!IPFromString(extaddr_str, &ext_addr)) {
		std::cerr << "Unable to parse IP address: " << extaddr_str << std::endl;
		return 1;
	}

	rtc::Thread* main = rtc::Thread::Current();
	rtc::AsyncUDPSocket* int_socket = rtc::AsyncUDPSocket::Create(main->socketserver(), int_addr);
	if (!int_socket) {
		std::cerr << "Failed to create a UDP socket bound at"
			<< int_addr.ToString() << std::endl;
		return 1;
	}

	rtc::AsyncSocket* tcp_server = main->socketserver()->CreateAsyncSocket(int_tcp_addr.family(), SOCK_STREAM);
	if (!tcp_server) {
		std::cerr << "Failed to create a TCP socket bound at"
			<< int_tcp_addr.ToString() << std::endl;
		return 1;
	}
	if (tcp_server->Bind(int_tcp_addr) != 0) {
		std::cerr << "Failed to bind a TCP socket bound at"
			<< int_tcp_addr.ToString() << std::endl;
		return 1;
	}
	if (tcp_server->Listen(5) != 0) {
		std::cerr << "Failed to listen a TCP socket bound at"
			<< int_tcp_addr.ToString() << std::endl;
		return 1;
	}

	char cmd[256];
	sprintf(cmd, "ps -ef | grep %s | grep -v \"grep\" | grep -v \".sh\" | awk '{print $2}'", argv[0]);
	FILE *fp = popen(cmd, "r");//打开管道，执行shell 命令
	char buffer[10] = {0};
	while (NULL != fgets(buffer, 10, fp)) //逐行读取执行结果并打印
	{
		pid = atoi(buffer);
		std::cout << "PID:  " << buffer << " " << pid << std::endl;
	}
	pclose(fp);

	std::cout << gettimestring() << "main thread id " << main->GetPThread() << std::endl;
	cricket::TurnServer server(main);
	TurnFileAuth auth(argv[5]);
	server.set_realm(argv[4]);
	server.set_software(WorkerConstants::kSoftware);   
	server.set_auth_hook(&auth);

	server.AddInternalSocket(int_socket, cricket::PROTO_UDP);
	std::cout << "Listening udp internally at " << int_addr.ToString() << std::endl;

	server.AddInternalServerSocket(tcp_server, cricket::PROTO_TCP);
	std::cout << "Listening tcp internally at " << int_tcp_addr.ToString() << std::endl;

	server.SetExternalSocketFactory(new rtc::BasicPacketSocketFactory(),
			rtc::SocketAddress(ext_addr, 0));

	std::unique_ptr<rtc::Thread> worker = rtc::Thread::CreateWithSocketServer();
	ElbsConnection elbs(worker.get());
	elbs.start();

	rtc::SocketAddress ClientAddr("127.0.0.1", 0);
	rtc::SocketAddress ServerAddr("127.0.0.1", 3002);
	Worker testWorker(worker.get(), &server, &elbs);
	testWorker.AddWorkerManagerSocket(ClientAddr, ServerAddr);
	worker->Start();

	std::cout << gettimestring() << "===== worker start!" << std::endl;

	base::EventLoop network_loop;
	UniLbsWorker turn_worker(&network_loop, &server, udpport);
	turn_worker.Run();

	main->Run();

	std::cout << "main thread quit!" << std::endl;
	return 0;
}
