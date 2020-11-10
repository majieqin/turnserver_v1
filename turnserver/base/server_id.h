// Copyright (c) 2014-2017 Agora.io, Inc.
// Server types

#pragma once  // NOLINT(build/header_guard)

namespace agora {
namespace base {

// clang-format off
enum ServiceType {
  kVoiceCenterType           = 0,
  kVoiceServerType           = 1,
  kVoiceTestType             = 2,
  kVoiceRouterType           = 3,
  kVomSignalType             = 4,
  kVomVosType                = 5,
  kVoqaType                  = 6,
  kBalancerType              = 7,
  kVoiceSyncType             = 8,
  kXcodecServerType          = 9,
  kVoiceRelayType            = 10,
  kBalancer2Type             = 11,
  kRuleRuleServerType        = 12,
  kRuleAuthServerType        = 13,
  kAuthVoiceServerType       = 14,
  kStunServerType            = 15,
  kCopsServerType            = 16,
  kUniLbsType                = 17,
  kCdsType                   = 18,
  kCdsLoaderType             = 19,
  kBroadcastType             = 20,
  kTdsType                   = 21,
  kTdsLoaderType             = 22,
  kAutoRebalancerType        = 23,
  kAutoRebalancerHostType    = 24,
  kStringUidAllocatorType    = 25,
  kVoserverBlackboxTestType  = 26,

  kCdnLbsType                = 30,
  kCdnReportType             = 31,
  kCdnRecorderType           = 32,
  kCdnLbsEventType           = 33,

  kVoiceRobotCtrlType        = 50,

  kOnlineCollrType           = 60,
  kOnlineCalcType            = 61,

  kSubscribeType             = 70,

  kVoiceRegistryType         = 71,

  kCthunAgentType            = 72,
  kCthunServerType           = 73,

  kReportServerType          = 100,
  kIceServerType             = 200,
  kHiveEdgeServerType        = 300,
  kHiveRelayServerType       = 301,
  kHiveWriterServerType      = 302,
  kHiveBalancerType          = 303,
  kHiveCenterType            = 304,
  kStatsEslbServerType       = 305,
  kReportEdgeType            = 306,
  kReportRelayType           = 307,
  kReportWriterType          = 308,
  kReportDbLoaderType        = 309,

  kConfigServerType          = 400,

  kSignalingRegistrarType    = 410,
  kSignalingForwarderType    = 411,
  kSignalingOrganizerType    = 412,
  kSignalingDispenserType    = 413,
  kSignalingDistributorType  = 414,
  kSignalingUserwatchType    = 415,
  kSignalingGroupwatchType   = 416,
  kSignalingMsgcacheType     = 417,
  kSignalingMsgarchiveType   = 418,
  kSignalingUsernoterType    = 419,
  kSignalingGroupnoterType   = 420,
  kSignalingRosterType       = 421,
  kSignalingCourierType      = 422,
  kSignalingConfigAgentType  = 430,
  kSignalingConfigServerType = 431,

  kVocsProxyType             = 500,
  kAccessPointType           = 501,

  kCDNDispatcherServerType   = 600,

  kMpUdpType                 = 700,

  kAppCenterServerType       = 1000,
  kUapRESTyType              = 1001,

  // kRelayType is reserved for basic IP-core protocols
  kRelayType                 = 65535,
};
// clang-format on

}  // namespace base
}  // namespace agora

// clang-format off
// uni_lbs support service
#define LBS_VOICE_SERVICE                    "voice_service"
#define LBS_WEBRTC_SERVICE                   "webrtc_service"
#define LBS_P2P_SERVICE                      "stun_service"
#define LBS_REPORT_SERVICE                   "report_service"
#define LBS_SIGNAL_SERVICE                   "signal_service"
#define LBS_SIGNAL_GATEWAY_SERVICE           "signal_gateway_service"
#define LBS_VOET_SERVICE                     "voet_service"
#define LBS_WEBRTC_V2_SERVICE                "webrtc_v2_service"
#define LBS_AUT_VOICE_SERVICE                "aut_voice_service"

#define FLAG_FILE_SUFFIX                     ".flags"
#define GLOBAL_FLAG_PREFIX                   "/etc/voice/global"
#define GLOBAL_FLAGS_FILE                    GLOBAL_FLAG_PREFIX FLAG_FILE_SUFFIX
#define GLOBAL_FLAGS_DIR                     "/etc/voice/"

#define VOICE_SERVER_STRING                  "/voice_server"
#define VOET_SERVER_STRING                   "/voet_server"
#define VOICE_TEST_STRING                    "/voice_test"
#define VOICE_ROUTER_STRING                  "/voice_router"
#define VOICE_BALANCER_STRING                "/voice_balancer"
#define VOICE_ONLINE_STRING                  "/voice_online"
#define VOICE_CHANNEL_SERVICE_STRING         "/voice_channel_service"
#define VOICE_RBT_CTRL_STRING                "/voice_robot_ctrl_service"
#define VOICE_SYNC_STRING                    "/voice_sync_service"
#define VIDEO_XCODEC_STRING                  "/video_xcodec_service"
#define HIVE_EDGE_SERVER_STRING              "/hive_edge_service"
#define HIVE_RELAY_SERVER_STRING             "/hive_relay_service"
#define HIVE_WRITER_SERVER_STRING            "/hive_writer_service"
#define REPORT_EDGE_STRING                   "/report_edge"
#define REPORT_RELAY_STRING                  "/report_relay"
#define REPORT_WRITER_STRING                 "/report_writer"
#define DATA_IN_STRING                       "/data_in"
#define DATA_OUT_STRING                      "/data_out"
#define CDN_EDGE_SERVER_STRING               "/cdn_edge_server"
#define STATS_ESLB_SERVER_STRING             "/stat_eslb_service"
#define VOICE_TEST2_STRING                   "/voice_test2"
#define BALANCER_PROXY_STRING                "/balancer_proxy"

#define CTHUN_AGENT                          "/cthun_agent"
#define CTHUN_CENTER                         "/cthun_center"

#define WEBRTC_SERVER_STRING                 "/webrtc_server"

#define ONLINE_STATS_COLLECTOR_STRING        "/online_collr"
#define ONLINE_STATS_CALCULATOR_STRING       "/online_calc"

#define CONFIG_SERVER_STRING                 "/config_server"
#define AUTH_SERVER_STRING                   "/auth_server"
#define VOCS_PROXY_STRING                    "/vocs_proxy"
#define LBES_STRING                          "/lbes"
#define DISPATCHER_STRING                    "/dispatcher"

#define STUN_SERVER_STRING                   "/stun_server"

#define VOICS_LBS_SERVER_STRING              "/voice_lbs_server"
#define WEBRTC_LBS_SERVER_STRING             "/webrtc_lbs_server"
#define P2P_LBS_SERVER_STRING                "/p2p_lbs_server"
#define VOICS_LBS_SERVER_STRING_V2           "/voice_lbs_server_v2"
#define WEBRTC_LBS_SERVER_STRING_V2          "/webrtc_lbs_server_v2"
#define WEBRTC2_LBS_SERVER_STRING_V2         "/webrtc2_lbs_server_v2"
#define P2P_LBS_SERVER_STRING_V2             "/p2p_lbs_server_v2"
#define CDS_SERVER_STRING                    "/cds_server"
#define CDN_DISPATCHER_SERVER_STRING         "/cdn_dispatcher_server"
#define ACCESS_POINT_PROXY_WHITE_LIST_STRING "/ap_ip_white_list"
#define REPORT_LBS_SERVER_STRING_V2          "/report_lbs_server_v2"
#define TDS_SERVER_STRING                    "/tds_server"
#define SUA_SERVER_STRING                    "/sua_server"
#define SIG_ENV0_LBS_SERVER_STRING_V2        "/sig_env0_lbs_server_v2"
#define SIG_ENV1_LBS_SERVER_STRING_V2        "/sig_env1_lbs_server_v2"
#define VOET_LBS_SERVER_STRING_V2            "/voet_lbs_server_v2"
#define SIG_LBS_SERVER_STRING_V2             "/sig_lbs_server_v2"
#define APP_CENTER_SERVER_STRING             "/uap_app_center_server"
#define SIGNALING_REGISTRAR_STRING           "/signaling_registrar"
#define SIGNALING_ROSTER_STRING              "/signaling_roster"
#define SIGNALING_FORWARDER_STRING           "/signaling_forwarder"
#define SIGNALING_DISPENSER_STRING           "/signaling_dispenser"
#define SIGNALING_ORGANIZER_STRING           "/signaling_organizer"
#define SIGNALING_DISTRIBUTOR_STRING         "/signaling_distributor"
#define SIGNALING_USERWATCH_STRING           "/signaling_userwatch"
#define SIGNALING_GROUPWATCH_STRING          "/signaling_groupwatch"
#define SIGNALING_QA_STRING                  "/signaling_qa"
#define SIGNALING_MSGCACHE_STRING            "/signaling_msgcache"
#define SIGNALING_MSGARCHIVE_STRING          "/signaling_msgarchive"
#define SIGNALING_GATEWAY_STRING             "/signaling_gateway"
#define AUTO_REBALANCER_STRING               "/auto_rebalancer"
#define AUTO_REBALANCER_HOST_STRING          "/auto_rebalancer_host"
#define FLV_SERVER_STRING                    "/flv_service"
// clang-format on
