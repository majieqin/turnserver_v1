// Copyright (c) 2019 Agora.io, Inc.
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <tuple>
#include <type_traits>

namespace agora {
namespace base {

namespace internal {

template<typename... Ts>
struct make_void {
  using type = void;
};

}  // namespace internal

template<typename... Ts>
using void_t = typename internal::make_void<Ts...>::type;

// +-----------------------+
// |  template interfaces  |
// +-----------------------+
template<typename T, typename = void>
struct IsIterable : std::false_type {};

template<typename T>
struct IsIterable<T, void_t<decltype(std::declval<T>().begin()),
                            decltype(std::declval<T>().end())>>
    : std::true_type {};

// +-----------------------+
// |  template utilities   |
// +-----------------------+

/// Variadic logical AND metafunction
template<typename...>
struct Conjunction : std::true_type {};
template<typename C>
struct Conjunction<C> : C {};
template<typename C0, typename... Rest>
struct Conjunction<C0, Rest...>
    : std::conditional<bool(C0::value), Conjunction<Rest...>, C0>::type {};

/// Variadic logical OR metafunction
template<typename...>
struct Disjunction : std::false_type {};
template<typename C>
struct Disjunction<C> : C {};
template<typename C0, typename... Rest>
struct Disjunction<C0, Rest...>
    : std::conditional<bool(C0::value), C0, Disjunction<Rest...>>::type {};

/// Logical NOT metafunction
template<class B>
struct Negation : std::integral_constant<bool, !static_cast<bool>(B::value)> {};

template<typename T, typename... TList>
struct Contains : Disjunction<std::is_same<T, TList>...> {};

/// Variadic Template Type List Operations
template<typename... Ts>
struct TypeList;

namespace detail {

template<std::size_t I, typename... Ts>
struct type_list_idx_impl;

template<typename T, typename... Rest>
struct type_list_idx_impl<0, T, Rest...> {
  using type = T;
};

template<std::size_t I, typename T, typename... Rest>
struct type_list_idx_impl<I, T, Rest...> {
  using type = typename type_list_idx_impl<I - 1, Rest...>::type;
};

template<std::size_t I, typename... Ts>
struct type_list_idx_helper {
  static_assert(I < sizeof...(Ts),
                "index must be smaller than number of types");

  using type = type_list_idx_impl<I, Ts...>;
};

template<typename T, typename U>
struct extend_helper;

template<typename... Ts, typename... Us>
struct extend_helper<TypeList<Ts...>, TypeList<Us...>> {
  using type = TypeList<Ts..., Us...>;
};

template<std::size_t I, typename Types>
struct prefix_helper {
  using type = typename prefix_helper<I - 1, Types>::type::template PushBack<
      typename Types::template Type<I - 1>>;
};

template<typename Types>
struct prefix_helper<1, Types> {
  using type = TypeList<typename Types::template Type<0>>;
};

template<typename Types>
struct prefix_helper<0, Types> {
  using type = TypeList<>;
};

template<std::size_t I, typename Types>
struct suffix_helper {
  using type = typename suffix_helper<I - 1, Types>::type::template PushFront<
      typename Types::template Type<Types::Size - I>>;
};

template<typename Types>
struct suffix_helper<1, Types> {
  using type = TypeList<typename Types::template Type<Types::Size - 1>>;
};

template<typename Types>
struct suffix_helper<0, Types> {
  using type = TypeList<>;
};

template<typename InitValue, template<typename, typename> class Op,
         std::size_t I, typename Types>
struct fold_expr_helper {
  using type = Op<typename fold_expr_helper<InitValue, Op, I - 1, Types>::type,
                  typename Types::template Type<I - 1>>;
};

template<typename InitValue, template<typename, typename> class Op,
         typename Types>
struct fold_expr_helper<InitValue, Op, 1, Types> {
  using type = Op<InitValue, typename Types::template Type<0>>;
};

template<typename InitValue, template<typename, typename> class Op,
         typename Types>
struct fold_expr_helper<InitValue, Op, 0, Types> {
  using type = InitValue;
};

template<std::size_t I, template<typename> class Op, typename Types>
struct transform_helper {
 private:
  using _prefix = typename transform_helper<I - 1, Op, Types>::type;

 public:
  using type = typename _prefix::template PushBack<
      Op<typename Types::template Type<I - 1>>>;
};

template<template<typename> class Op, typename Types>
struct transform_helper<1, Op, Types> {
  using type = TypeList<Op<typename Types::template Type<0>>>;
};

template<template<typename> class Op, typename Types>
struct transform_helper<0, Op, Types> {
  using type = TypeList<>;
};

template<typename T, std::size_t N, typename... Args>
struct index_helper;

template<typename T, std::size_t N, typename First, typename... Rest>
struct index_helper<T, N, First, Rest...>
    : std::conditional<std::is_same<T, First>::value,
                       std::integral_constant<std::size_t, N>,
                       index_helper<T, N + 1, Rest...>>::type {};

template<typename T, std::size_t N, typename First>
struct index_helper<T, N, First> : std::integral_constant<std::size_t, N> {
  static_assert(std::is_same<T, First>::value, "type is not in type list");
};

}  // namespace detail

template<typename... Ts>
struct TypeList {
  using standard_tuple = std::tuple<Ts...>;

  template<std::size_t I>
  using Type = typename detail::type_list_idx_helper<I, Ts...>::type::type;

  static const std::size_t Size = sizeof...(Ts);

  template<typename T>
  using PushBack = TypeList<Ts..., T>;

  template<typename T>
  using PushFront = TypeList<T, Ts...>;

  template<typename T>
  using Extend = typename detail::extend_helper<TypeList, T>::type;

  template<std::size_t N>
  using Prefix = typename detail::prefix_helper<N, TypeList>::type;

  template<std::size_t N>
  using Suffix = typename detail::suffix_helper<N, TypeList>::type;

  template<typename T>
  using Index = typename detail::index_helper<T, 0, Ts...>;

  template<template<typename> class Op>
  using ConjunctionOp = Conjunction<Op<Ts>...>;

  template<template<typename> class Op>
  using DisjunctionOp = Disjunction<Op<Ts>...>;

  template<template<typename> class Op>
  using Transform = typename detail::transform_helper<Size, Op, TypeList>::type;

  // fold expression, default left
  template<typename InitValue, template<typename, typename> class Op>
  using FoldExpression =
      typename detail::fold_expr_helper<InitValue, Op, Size, TypeList>::type;
};

template<typename... Ts>
const std::size_t TypeList<Ts...>::Size;

}  // namespace base
}  // namespace agora
