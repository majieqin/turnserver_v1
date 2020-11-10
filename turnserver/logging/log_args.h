// Copyright (c) 2014-2017 Agora.io, Inc.
// media-server@agora.io
// easy log variable name and value
//

#pragma once  // NOLINT(build/header_guard)


#define LOG_FOR_EACH_1(Y, X) Y(X)
#define LOG_FOR_EACH_2(Y, X, ...) Y(X) << ", " << LOG_FOR_EACH_1(Y, __VA_ARGS__)
#define LOG_FOR_EACH_3(Y, X, ...) Y(X) << ", " << LOG_FOR_EACH_2(Y, __VA_ARGS__)
#define LOG_FOR_EACH_4(Y, X, ...) Y(X) << ", " << LOG_FOR_EACH_3(Y, __VA_ARGS__)
#define LOG_FOR_EACH_5(Y, X, ...) Y(X) << ", " << LOG_FOR_EACH_4(Y, __VA_ARGS__)
#define LOG_FOR_EACH_6(Y, X, ...) Y(X) << ", " << LOG_FOR_EACH_5(Y, __VA_ARGS__)
#define LOG_FOR_EACH_7(Y, X, ...) Y(X) << ", " << LOG_FOR_EACH_6(Y, __VA_ARGS__)
#define LOG_FOR_EACH_8(Y, X, ...) Y(X) << ", " << LOG_FOR_EACH_7(Y, __VA_ARGS__)
#define LOG_FOR_EACH_9(Y, X, ...) Y(X) << ", " << LOG_FOR_EACH_8(Y, __VA_ARGS__)


#define LOG_GET_FOR_EACH(_1, _2, _3, _4, _5, _6, _7, _8, _9, WHICH, ...) WHICH


#define LOG_FOR_EACH(action, ...) LOG_GET_FOR_EACH(__VA_ARGS__, \
    LOG_FOR_EACH_9, \
    LOG_FOR_EACH_8, \
    LOG_FOR_EACH_7, \
    LOG_FOR_EACH_6, \
    LOG_FOR_EACH_5, \
    LOG_FOR_EACH_4, \
    LOG_FOR_EACH_3, \
    LOG_FOR_EACH_2, \
    LOG_FOR_EACH_1)(action, __VA_ARGS__)


#define LOG_ARG(v) #v << ": " << (v)


#define LOG_ARGS(...) LOG_FOR_EACH(LOG_ARG, __VA_ARGS__)

