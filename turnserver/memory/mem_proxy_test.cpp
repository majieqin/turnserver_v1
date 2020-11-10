// Copyright (c) 2019 Agora.io, Inc.
//

#include "media_server_library/memory/mem_proxy.h"

#include <string>
#include <type_traits>

#include "gtest/gtest.h"
#include "memory/mem_buf.h"

namespace agora {
namespace memory {
namespace test {

struct POD_A {  // 8
  uint32_t f1;
  uint32_t f2;
} __attribute__((packed));

struct POD_B {  // 16
  uint32_t f1;
  uint32_t f2;
  uint64_t f3;
} __attribute__((packed));

struct POD_C {  // 20
  uint32_t f1;
  uint32_t f2;
  uint32_t f3;
  uint64_t f4;
} __attribute__((packed));

struct NonPOD {
  std::string str;
};

using CommonProxy = MemProxy<POD_A, POD_B, POD_C>;

TEST(MemProxyTest, construct) {
  // compile passed
  MemProxy<POD_A, POD_B, POD_C> p;
  (void)p;

  // should not compile
  // MemProxy<POD_A, POD_B, POD_C, NonPOD> q;
}

#define EXPECT_TRUE2(...) \
  {                       \
    auto v = __VA_ARGS__; \
    EXPECT_TRUE(v);       \
  }

TEST(MemProxyTest, static_test) {
  CommonProxy p;
  (void)p;

  EXPECT_TRUE2(std::is_same<CommonProxy::NthArg<0>, POD_A>::value);
  EXPECT_TRUE2(std::is_same<CommonProxy::NthArg<1>, POD_B>::value);
  EXPECT_TRUE2(std::is_same<CommonProxy::NthArg<2>, POD_C>::value);

  EXPECT_EQ(CommonProxy::FullSize::value, 44);

  EXPECT_EQ(CommonProxy::PrefixSize<0>::value, 0);
  EXPECT_EQ(CommonProxy::PrefixSize<1>::value, 8);
  EXPECT_EQ(CommonProxy::PrefixSize<2>::value, 24);
  EXPECT_EQ(CommonProxy::PrefixSize<3>::value, 44);

  EXPECT_EQ(CommonProxy::SuffixSize<0>::value, 0);
  EXPECT_EQ(CommonProxy::SuffixSize<1>::value, 20);
  EXPECT_EQ(CommonProxy::SuffixSize<2>::value, 36);
  EXPECT_EQ(CommonProxy::SuffixSize<3>::value, 44);
}

TEST(MemProxyTest, layer_test) {
  auto p = MakeProxy<POD_A, POD_B, POD_C>();

  EXPECT_EQ(p.Layer<0>(), p.Layer<POD_A>());
  EXPECT_EQ(p.Layer<1>(), p.Layer<POD_B>());
  EXPECT_EQ(p.Layer<2>(), p.Layer<POD_C>());
}

TEST(MemProxyTest, borrow_small_buffer) {
  auto slice1 = memory::MemBuf::Create(10);
  auto slice2 = memory::MemBuf::Create(14);

  auto func = [=]() {
    auto p = MakeProxy<POD_A, POD_B, POD_C>(memory::MemSliceSpan{
        memory::MemSlice{slice1}, memory::MemSlice{slice2}});
  };

  // should crash
  (void)func;
}

TEST(MemProxyTest, borrow_direct_use) {
  auto slice1 = memory::MemBuf::Create(10);
  auto slice2 = memory::MemBuf::Create(34);

  auto p = MakeProxy<POD_A, POD_B, POD_C>(
      memory::MemSliceSpan{memory::MemSlice{slice1}, memory::MemSlice{slice2}});

  EXPECT_EQ(p.Layer<0>(), slice1->Begin());
  ASSERT_EQ(p.GetBuffer().GetBlockCount(), 2);

  EXPECT_EQ(p.Layer<2>(),
            reinterpret_cast<const POD_C *>(
                reinterpret_cast<const char *>(slice2->Begin()) + 14));
  EXPECT_EQ(p.GetBuffer().GetBlockCount(), 2);
}

TEST(MemProxyTest, borrow_merge_buffer) {
  auto slice1 = memory::MemBuf::Create(10);
  auto slice2 = memory::MemBuf::Create(34);

  auto p = MakeProxy<POD_A, POD_B, POD_C>(
      memory::MemSliceSpan{memory::MemSlice{slice1}, memory::MemSlice{slice2}});

  auto l = p.Layer<1>();
  EXPECT_EQ(l, reinterpret_cast<const POD_B *>(
                   reinterpret_cast<const char *>(
                       p.GetBuffer().GetFirstBlock().Begin()) +
                   8));
  EXPECT_EQ(p.GetBuffer().GetBlockCount(), 1);
}

}  // namespace test
}  // namespace memory
}  // namespace agora

