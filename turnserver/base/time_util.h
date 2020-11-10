// Copyright (c) 2014-2017 Agora.io, Inc.
// TODO(liuyong): requires a detailed description
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>

// get a strict non-decreasing tick
namespace agora {
namespace base {

int64_t TickMicroSeconds();
int64_t TickMilliSeconds();
int32_t TickSeconds();

int64_t TickInMicroSeconds();
int64_t TickInMilliSeconds();
int32_t TickInSeconds();

int64_t HighResolutionTickInMicroSeconds();
int64_t HighResolutionTickInMilliSeconds();
int32_t HighResolutionTickInSeconds();

int32_t NowInSeconds();
int64_t NowInMilliSeconds();
int64_t NowInMicroSeconds();

}  // namespace base
}  // namespace agora
