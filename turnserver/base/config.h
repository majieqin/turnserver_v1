// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
//
// Common server configurations

#pragma once  // NOLINT(build/header_guard)

#define CENTER_HOST "center.voice.agora.com"
#define REPORT_SERVER_HOST "report.voice.agora.com"
#define VOM_HOST "vom.voice.agora.com"
#define VOQA_HOST "voqa.voice.agora.com"
#define VOQA2_HOST "voqa2.voice.agora.com"
#define VOS_HOST "vos.voice.agora.com"
#define VOCS_HOST "vocs.voice.agoralab.co"
#define VOO_HOST "voo.voice.agoralab.co"
#define RULE_SERVER_HOST "rule.voice.agoralab.co"
#define STATS_ESLB_HOST "qoslbs.agoralab.co"
#define VOPCU_HOST "vopcu.voice.agoralab.co"
#define ARGUS_HOST "qos.agoralab.co"
#define VOBALANCER_HOST "volbs.voice.agoralab.co"
#define VOBALANCER2_HOST "volbs2.voice.agoralab.co"
#define VOICE_SYNC_HOST "vosync.voice.agoralab.co"
#define VOICE_REGISTER_HOST "voregister.voice.agoralab.co"

#define CTHUN_CENTER_HOST "center.cthun.agoralab.co"

#define ICE_HOST "ice.agoralab.co"
#define ICE_STAT_HOST "stat.ice.agoralab.co"
#define STUN_HOST "stun.agoralab.co"
#define COPS_HOST "cops.agoralab.co"
#define STATS_BALANCER_HOST "statlbs.voice.agoralab.co"
#define STATS_CENTER_HOST "statcenter.voice.agoralab.co"

#define STATS_HIVE_HOST "writer.stats.agoralab.co"
#define STATS_INSTANT_AVAILABILITY_HOST "ha.stats.agoralab.co"
#define STATS_VOQA_HOST "voqawriter.stats.agoralab.co"
#define REPORT_HIVE_HOST "hive.report.agoralab.co"
#define REPORT_BALANCER_HOST "balancer.report.agoralab.co"
#define CDN_LBS_HOST "lbs.cdn.agoralab.co"
#define CDN_REPORT_HOST "report.cdn.agoralab.co"

#define SIG_FORWARDER_HOST "forwarder.sig.agoralab.co"

#define CONFIG_HOST "config.voice.agora.co"

#define ONLINE_COLLR_HOST "collr.online.agoralab.co"
#define ONLINE_REDIS_HOST "redis.online.agoralab.co"
#define ONLINE_CALC_HOST "calc.online.agoralab.co"

#define ISP_IP_KEY "isp_ip"
#define IDC_KEY "idc"
#define XCODE "xcode"
#define MACHINE_ID_KEY "id"

// uni_lbs
#define LBS_WEBRTC_HOST "webrtc.lbs.agoralab.co"
#define LBS_STUN_HOST "stun.lbs.agoralab.co"
#define LBS_VOICE_HOST "voice.lbs.agoralab.co"
#define LBS_REPORT_HOST "report.lbs.agoralab.co"
#define LBS_SIGNAL_HOST "signal.lbs.agoralab.co"
#define LBS_SIGNAL_GATEWAY_HOST "signal_gateway.lbs.agoralab.co"

namespace agora {
namespace base {

enum CoreServicePort {
  kCenterPort = 2000,  // voc.exe
  kCenterConsolePort = 12000,

  kReportServerPort = 2100,  // obsolescent

  kLbsEdgeServicePort = 3100,
  kLbsProxyServicePort = 2800,  // obsolescent
  kLbsConsolePort = 13100,
  kLbsSubPort = 2302,

  kStringUidAllocatorPort = 3200,  // string uid allocator

  kVocsPort = 2300,

  kVoqaPort = 2400,
  kVoqaConsolePort = 12400,

  kVoqa2Port = 2500,
  kVoqa2ConsolePort = 12500,

  kBalancerPort = 2700,
  kBalancerConsolePort = 12700,

  kBalancer2Port = 2701,
  kBalancer2BroadcastPort = 2702,
  kBalancer2Broadcast2Port = 2703,
  kBalancer2ConsolePort = 12701,

  kAuthServerPort = 2900,
  kRuleServerPort = 2950,
  kRuleServerHttpPort = 2951,
  kRuleServerPeerPort = 2960,

  kVosyncPort = 3500,
  kVosyncConsolePort = 13500,

  kCapSyncPort = 3501,
  kCapSyncInterPort = 3502,
  kCapSyncConsolePort = 13500,

  kCopsPort = 7500,
  kCopsSyncPort = 7550,
  kCopsInformerPort = 9800,
  kCopsOnlinePort = 7600,

  kCdsPort = 8100,
  kCdsLoaderPort = 8110,

  kTdsPort = 8150,
  kTdsLoaderPort = 8160,

  kRobotCenterPort = 8200,

  kInformerVosPort = 8600,

  kVocsClientPort = 25000,      // For SDK clients
  kIpv6VocsClientPort = 25200,  // For SDK clients

  kVocsP2pPort = 25100,
  // VOCS_CONSOLE_PORT = 35000,

  kVooPort = 26000,
  kVooConsolePort = 36000,

  kVopcuPort = 26500,

  kBroadcastMonitoringPort = 28000,
  kBroadcastMonitoringProxyPort = 29000,
  kBroadcastMonitoringConsolePort = 38000,

  kIcePort = 3000,
  kIceConsolePort = 13000,
  kIceStatsPort = 143,

  // FIXME(liuyong): Following ports are unknown
  kDataInPort = 1080,    // whose?
  kDataOutPort = 11080,  // whose?
  kDataInProxyTcpPort = 10080,
  kDataInProxySslPort = 10443,
  kDataInJiraProxySslPort = 20037,

  // obsolescent
  kCdnLbsPort = 27000,
  kCdnLbsConsolePort = 37000,
  kCdnLbsEventPort = 37200,

  // For report services
  kStatsBalancerPort = 9500,
  kStatsEslbServicePort = 9600,

  // For auto re-balancer
  kAutoRebalancerPort = 9900,
  kAutoRebalancerConsolePort = 9901,
  kAutoRebalancerHostPort = 9910,
  kAutoRebalancerHostConsoloPort = 9911,
  kAutoRebalancerHostHttpPort = 9999,

  kConfigServerPort = 15000,
  kConfigServer2Port = 15001,

  // For voice service register
  kVoregistryPort = 17000,
  kVoregistryQueryPort = 17001,

  // For access point
  kAccessPointConsolePort = 40000,
};

enum EdgeServicePort {
  // NOTE(liuyong): VosProxy only listens on localhost.
  kBalancerProxyPort = 4500,
  kBalancerProxyConsolePort = 4501,

  kVosServicePort = 4001,  // Start of voserver ports
  kVosServiceIpv6Port = 4101,
  kVotestServicePort = 5000,
  kVotest2ServicePort = 5100,
  kRouterServicePort = 6000,  // Start of vorouter ports

  kStunServicePort = 7000,
  kStunConsolePort = 7001,

  kReportEdgeServicePort = 8913,  // 8000 deprecated
  kReportEdgeSelectorPort = 8010,
  kReportInternalTransPort = 8020,
  kReportDataBaseLoaderPort = 8030,
  kReportBalancerServicePort = 8090,
  kReportBalancerBroadcastPort = 8091,

  kReportSelectorServicePort = 9700,

  kSignalingGatewayPort = 10000,  // 10000 - 12999
  kSignalingTokenServerPort = 12001,

  kCthunAgentPort = 13000,  // Cthun serivce 13000 - 13999
  kCthunServerPort = 13001,

  kCdnDispatcherPort = 3333,
  kCdnDispatcherConsolePort = 39000,

  kInstantAvailabilityServicePort = 9200,
  kInstantAvailabilityServiceConsolePort = 19200,
  kInstantAvailabilityKafkaServiceConsolePort = 19201,

  kWebGatewayWebSocketPort = 5866,  // Start of web gateway ports
};

enum SignalingServicePort {
  kSignalingRestfulPort = 9129,

  kRegistrarServicePort = 9130,  // & 9131
  kRegistrarProbingPort = 9132,  // & 9133
  kRosterRegistrarPort  = 9134,  // & 9135

  kForwarderRegistrarPort = 9141,
  kForwarderForwarderPort = 9142,

  kDistributorRegistrarPort = 9151,
  kDistributorForwarderPort = 9152,
  kDistributorDistributorPort = 9154,
  kDistributorOrganizerPort = 9153,

  kOrganizerRegistrarPort = 9161,

  kUserwatchRegistrarPort = 9171,  // & 9172
  kUserwatchForwarderPort = 9173,  // & 9174
  kUsernoterUserwatchPort = 9175,
  kUserwatchUserwatchPort = 9177,  // & 9178

  kGroupwatchRegistrarPort  = 9181,  // & 9182
  kGroupnoterForwarderPort  = 9183,  // & 9184
  kGroupnoterGroupwatchPort = 9185,
  kGroupwatchGroupwatchPort = 9187,  // & 9188

  kMsgcacheForwarderPort = 9191,
  kMsgcacheRedisPort     = 9192,

  kCourierServicePort = 9211,
  kCourierCourierPort = 9212,

  kSignalingKafkaPort = 9221,

  kSignalingCassandraClientPort = 9231,
  kSignalingCassandraStoragePort = 9232,

  kSignalingConfigServerPort = 9251,

  kWebGatewayPort = 9590,
};

enum OnlineStatsPort {
  // port for clients report
  kOnlineCollrClientPort = 2600,
  // port for online stats calculator subscribe
  kOnlineCollrSubscribePort = 2610,
  kOnlineCollrConsolePort = 12600,

  // port of first redis master node
  kOnlineRedisBasePort = 2621,

  kOnlineCalcPublishPort = 2660,

  // port of online_calc console = kOnlineCalcConsoleBasePort + partition
  kOnlineCalcConsoleBasePort = 12660,
};

}  // namespace base
}  // namespace agora
