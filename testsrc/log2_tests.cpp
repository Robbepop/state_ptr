#include <gtest/gtest.h>

#include <putl/state_ptr.hpp>

namespace {

using namespace putl;

TEST(ConstexprLog2, Zero) {
	EXPECT_EQ(detail::log2(0), 0);
}

TEST(ConstexprLog2, One) {
	EXPECT_EQ(detail::log2(1), 0);
}

TEST(ConstexprLog2, PowersOfTwo) {
	EXPECT_EQ(detail::log2(   2),  1);
	EXPECT_EQ(detail::log2(   4),  2);
	EXPECT_EQ(detail::log2(   8),  3);
	EXPECT_EQ(detail::log2(  16),  4);
	EXPECT_EQ(detail::log2(  32),  5);
	EXPECT_EQ(detail::log2(  64),  6);
	EXPECT_EQ(detail::log2( 128),  7);
	EXPECT_EQ(detail::log2( 256),  8);
	EXPECT_EQ(detail::log2( 512),  9);
	EXPECT_EQ(detail::log2(1024), 10);
}

TEST(ConstexprLog2, Odd) {
	EXPECT_EQ(detail::log2(13), 3);
	EXPECT_EQ(detail::log2(17), 4);
	EXPECT_EQ(detail::log2(35), 5);
}

} // namespace
