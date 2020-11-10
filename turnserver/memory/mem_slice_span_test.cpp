// Copyright (c) 2014-2020 Agora.io, Inc.
// Tests for |MemSliceSpan|.

#include <cstdlib>
#include <string>
#include <algorithm>
#include <utility>

#include "gtest/gtest.h"
#include "media_server_library/memory/mem_slice.h"
#include "media_server_library/memory/mem_slice_span.h"

namespace agora {
namespace memory {
namespace testing {

namespace {

void ExpectEq(const MemSliceSpan &lhs, const MemSliceSpan &rhs,
              bool permit_same_source = true) {
  EXPECT_EQ(lhs.GetUsedSize(), rhs.GetUsedSize());
  ASSERT_EQ(lhs.GetBlockCount(), rhs.GetBlockCount());

  for (std::size_t i = 0; i < lhs.GetBlockCount(); ++i) {
    if (!permit_same_source) {
      EXPECT_NE(lhs[i].GetMemBuf(), rhs[i].GetMemBuf());
    }

    // For now |MemSliceSpan| is read-only, so we only need to confirm the used
    // part. As for the confirmation of the entire |MemBuf| involved, it should
    // now be placed in the tests of |MemSlice|.
    ASSERT_EQ(lhs[i].GetUsedSize(), rhs[i].GetUsedSize());
    EXPECT_EQ(0,
              ::memcmp(lhs[i].Begin(), rhs[i].Begin(), lhs[i].GetUsedSize()));
  }
}

}  // unnamed namespace

TEST(MemSliceSpanTest, CloneEmptySpan) {
  MemSliceSpan empty_span;
  ExpectEq(empty_span, empty_span.Clone(), false);
}

TEST(MemSliceSpanTest, Clone) {
  auto char_generator = []() { return std::rand() % 255 + 1; };
  std::string payload_str(512, 0);
  std::generate(payload_str.begin(), payload_str.end(), char_generator);

  MemSliceSpan span;
  MemSlice slice1(MemBuf::Create(payload_str.data(), payload_str.size()));
  slice1.SetBeginPointer(10);
  slice1.SetEndPointer(500);
  span.PushBack(std::move(slice1));

  MemSlice slice2(MemBuf::Create(payload_str.data(), payload_str.size()));
  span.PushBack(std::move(slice2));

  ExpectEq(span, span.Clone(), false);
}

}  // namespace testing
}  // namespace memory
}  // namespace agora
