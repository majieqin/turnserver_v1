// Copyright (c) 2014-2017 Agora.io, Inc.
// Common error codes used especially by voserver to the user.

#pragma once  // NOLINT(build/header_guard)

namespace agora {
namespace base {

enum ErrorCode {
  // NOTE(Qi Ji): [0-10000) is for shared use
  // 10000 and above 10000 is reserved for each project
  // so DO NOT use error code larger than or equal to 10000
  kOk = 0,
  kFail = 1,
  kTimestampExpired = 2,
  kChannelPermissionInvalid = 3,
  kCertificateInvalid = 4,
  kChannelNameEmpty = 5,
  kChannelNotFound = 6,
  kTicketInvalid = 7,
  kChannelConflicted = 8,
  kServiceNotReady = 9,
  kServiceTooHeavy = 10,
  kRejoined = 11,

  kNoNeedToChange = 13,  // Only used by |ESLB| services

  kUidBanned = 14,
  kIpBanned = 15,
  kChannelBanned = 16,
  // for tickets
  kTicketParseError = 17,
  kTicketIllegalLength = 18,
  kTicketNotAuth    = 19,
  kTicketUnknownVersion = 20,

  kRequestPending = 65535,  // for internal use
};

}  // namespace base
}  // namespace agora
