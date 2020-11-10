// Copyright (c) 2019 Agora.io, Inc.
//

#pragma once  // NOLINT(build/header_guard)

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

#include "media_server_library/base/type_traits.hpp"
#include "media_server_library/memory/mem_buf.h"
#include "media_server_library/memory/mem_slice.h"
#include "media_server_library/memory/mem_slice_span.h"

namespace agora {
namespace memory {

namespace detail {

struct MemBufDeleter {
  inline void operator()(memory::MemBuf *buffer) const {
    buffer->Release();
  }
};

template<typename T, typename U>
struct Sum : std::integral_constant<std::size_t, T::value + U::value> {};

template<typename T>
struct Sizeof : std::integral_constant<std::size_t, sizeof(T)> {};

}  // namespace detail

template<typename... Ts>
struct MemProxy {
  static_assert(base::Conjunction<std::is_pod<Ts>...>::value,
                "all types must be POD");
  static_assert(sizeof...(Ts) > 0, "MemProxy must proxy at least one type");

  using Args = base::TypeList<Ts...>;

  template<std::size_t N>
  using NthArg = typename Args::template Type<N>;

  using FullSize = typename Args::template Transform<detail::Sizeof>::
      template FoldExpression<std::integral_constant<std::size_t, 0>,
                              detail::Sum>;

  template<std::size_t N>
  using PrefixSize =
      typename Args::template Prefix<N>::template Transform<detail::Sizeof>::
          template FoldExpression<std::integral_constant<std::size_t, 0>,
                                  detail::Sum>;

  template<std::size_t N>
  using SuffixSize =
      typename Args::template Suffix<N>::template Transform<detail::Sizeof>::
          template FoldExpression<std::integral_constant<std::size_t, 0>,
                                  detail::Sum>;
};

template<typename... Ts>
struct BorrowedMemProxy : public MemProxy<Ts...> {
  explicit BorrowedMemProxy(memory::MemSliceSpan buffer) : buffer_(buffer) {
    assert(buffer.GetUsedSize() >= MemProxy<Ts...>::FullSize::value);
  }

  template<std::size_t N>
  inline const typename MemProxy<Ts...>::Args::template Type<N> *Layer() const {
    if (buffer_.IsSingleBlock()) {
      const char *begin =
          static_cast<const char *>(buffer_.GetFirstBlock().Begin());
      const char *next = begin + MemProxy<Ts...>::template PrefixSize<N>::value;
      return reinterpret_cast<
          const typename MemProxy<Ts...>::Args::template Type<N> *>(next);
    } else {
      using PrefixSizeType = typename MemProxy<Ts...>::template PrefixSize<N>;
      auto skip_sz = PrefixSizeType::value;
      using ThisType = typename MemProxy<Ts...>::Args::template Type<N>;

      for (auto i = 0u; i < buffer_.GetBlockCount(); i++) {
        const char *begin = static_cast<const char *>(buffer_[i].Begin());

        if (buffer_[i].GetUsedSize() < skip_sz) {
          skip_sz -= buffer_[i].GetUsedSize();
          continue;
        }

        if (buffer_[i].GetUsedSize() < skip_sz + sizeof(ThisType)) {
          buffer_ = buffer_.ToSingleBlock();

          return Layer<N>();
        }

        const char *next = begin + skip_sz;
        return reinterpret_cast<
            const typename MemProxy<Ts...>::Args::template Type<N> *>(next);
      }
    }

    return nullptr;
  }

  template<typename T>
  inline const T *Layer() const {
    return Layer<MemProxy<Ts...>::Args::template Index<T>::value>();
  }

  inline memory::MemSliceSpan GetBuffer() const {
    return buffer_;
  }

 private:
  mutable memory::MemSliceSpan buffer_;
};

template<typename... Ts>
struct CreatedMemProxy : public MemProxy<Ts...> {
  CreatedMemProxy() {
    auto buf = memory::MemBuf::Create(MemProxy<Ts...>::FullSize::value);
    buffer_.reset(buf);
  }

  inline memory::MemSlice ToSlice() {
    assert(buffer_.get() != nullptr);
    return memory::MemSlice{buffer_.release()};
  }

  template<std::size_t N>
  inline typename MemProxy<Ts...>::Args::template Type<N> *Layer() {
    char *begin = static_cast<char *>(buffer_->Begin());
    char *next = begin + MemProxy<Ts...>::template PrefixSize<N>::value;
    return reinterpret_cast<typename MemProxy<Ts...>::Args::template Type<N> *>(
        next);
  }

  template<typename T>
  inline T *Layer() {
    return Layer<MemProxy<Ts...>::Args::template Index<T>::value>();
  }

 private:
  std::unique_ptr<memory::MemBuf, detail::MemBufDeleter> buffer_;
};

template<typename... Ts>
CreatedMemProxy<Ts...> MakeProxy() {
  return CreatedMemProxy<Ts...>{};
}

template<typename... Ts>
BorrowedMemProxy<Ts...> MakeProxy(memory::MemSliceSpan buffer) {
  return BorrowedMemProxy<Ts...>{buffer};
}

}  // namespace memory
}  // namespace agora

