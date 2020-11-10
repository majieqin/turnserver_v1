// Copyright (c) 2014-2017 Agora.io, Inc.
// Utilities in compile-time
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <string>
#include <tuple>
#include <type_traits>

#include "type_traits.hpp"

namespace agora {
namespace base {

template<typename T>
using Invoke = typename T::type;

template<uint32_t...>
struct Sequence {
  using type = Sequence;
};

template<typename S1, typename S2>
struct _ConcatSequence;

template<uint32_t... I1, uint32_t... I2>
struct _ConcatSequence<Sequence<I1...>, Sequence<I2...>>
    : Sequence<I1..., (sizeof...(I1) + I2)...> {};

template<typename S1, typename S2>
using ConcatSequence = Invoke<_ConcatSequence<S1, S2>>;

template<uint32_t N>
struct _GenSequence;

template<uint32_t N>
using GenSequence = Invoke<_GenSequence<N>>;

template<uint32_t N>
struct _GenSequence
    : ConcatSequence<GenSequence<N / 2>, GenSequence<N - N / 2>> {};

template<>
struct _GenSequence<0> : Sequence<> {};
template<>
struct _GenSequence<1> : Sequence<0> {};

template<typename T, std::size_t N, class... Args>
struct _GetIndexOfElementFromTupleImpl {
  static constexpr auto value = N;
};

template<typename T, std::size_t N, class... Args>
struct _GetIndexOfElementFromTupleImpl<T, N, T, Args...> {
  static constexpr auto value = N;
};

template<typename T, std::size_t N, typename U, class... Args>
struct _GetIndexOfElementFromTupleImpl<T, N, U, Args...> {
  static constexpr auto value =
      _GetIndexOfElementFromTupleImpl<T, N + 1, Args...>::value;
};

template<typename T, typename... Args>
T &GetElementByType(std::tuple<Args...> &t) {  // NOLINT
  return std::get<_GetIndexOfElementFromTupleImpl<T, 0, Args...>::value>(t);
}

template<typename... T>
struct TypeSet {
  template<typename U>
  using PushFront = TypeSet<U, T...>;
  template<typename U>
  using PushBack = TypeSet<T..., U>;
  template<template<typename... Args> class U>
  using rebind = U<T...>;
};

template<typename F, typename S>
struct FunctionArgumentsMatch;

template<typename F, typename... Args>
struct FunctionArgumentsMatch<F, TypeSet<Args...>> {
  template<typename F1, typename... Args1,
           typename = decltype(std::declval<F1>()(std::declval<Args1>()...))>
  static char __test(int);

  template<typename...>
  static int __test(int);

  static constexpr bool value = sizeof(__test<F, Args...>(0)) == sizeof(char);
};

template<int N>
struct SingleTag {};

template<>
struct SingleTag<1> {
  using type = int64_t;
};

template<>
struct SingleTag<2> {
  using type = uint64_t;
};

template<>
struct SingleTag<3> {
  using type = double;
};

template<>
struct SingleTag<4> {
  using type = std::string;
};

template<uint64_t Tag>
struct Arguments {
  using SubArguments = typename Arguments<Tag / 5>::type;
  using type =
      typename SubArguments::template push<typename SingleTag<Tag % 5>::type>;
};

template<>
struct Arguments<0> {
  using type = TypeSet<>;
};

template<typename... T>
struct LastElement {
  using type =
      typename std::tuple_element<sizeof...(T) - 1, std::tuple<T...>>::type;
};

template<>
struct LastElement<> {};

template<size_t N>
struct ConstantIndex : std::integral_constant<uint32_t, N> {};

template<typename TAG, size_t Rank, size_t Acc>
constexpr ConstantIndex<Acc> CounterCrumb(TAG, ConstantIndex<Rank>,
                                          ConstantIndex<Acc>) {
  return {};
}

#define COUNTER_LINK_NAMESPACE(NS) using NS::CounterCrumb;

template<typename T>
struct CounterTag {};

#define COUNTER_READ_CRUMB(TAG, RANK, ACC)           \
  CounterCrumb(::agora::base::CounterTag<TAG>{},     \
               ::agora::base::ConstantIndex<RANK>{}, \
               ::agora::base::ConstantIndex<ACC>{})
#define COUNTER_READ(TAG)                     \
  COUNTER_READ_CRUMB(                         \
      TAG, 1,                                 \
      COUNTER_READ_CRUMB(                     \
          TAG, 2,                             \
          COUNTER_READ_CRUMB(                 \
              TAG, 4,                         \
              COUNTER_READ_CRUMB(             \
                  TAG, 8,                     \
                  COUNTER_READ_CRUMB(         \
                      TAG, 16,                \
                      COUNTER_READ_CRUMB(     \
                          TAG, 32,            \
                          COUNTER_READ_CRUMB( \
                              TAG, 64, COUNTER_READ_CRUMB(TAG, 128, 0))))))))

#define COUNTER_INC(TAG)                                                      \
  constexpr ::agora::base::ConstantIndex<COUNTER_READ(TAG) + 1> CounterCrumb( \
      ::agora::base::CounterTag<TAG>,                                         \
      ::agora::base::ConstantIndex<(COUNTER_READ(TAG) + 1) &                  \
                                   ~COUNTER_READ(TAG)>,                       \
      ::agora::base::ConstantIndex<(COUNTER_READ(TAG) + 1) &                  \
                                   COUNTER_READ(TAG)>) {                      \
    return {};                                                                \
  }
}  // namespace base
}  // namespace agora

