#include <gtest/gtest.h>

#include <putl/state_ptr.hpp>

#include <vector>
#include <iostream>

namespace {

using namespace putl;

struct Foo{
	int a;
};

enum Bar {
	A = 0,
	B = 1,
	C = 2,
	D = 1337 // probably too large !
};

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

TEST(StatePointer, InitializedAsNull) {
	state_ptr<Foo> foo{nullptr, 0};
	state_ptr<Foo> bar{nullptr, 0};
	ASSERT_EQ(foo, bar);
	ASSERT_EQ(foo, nullptr);
}

TEST(StatePointer, GetStateNull) {
	Foo foo;
	state_ptr<Foo> p{&foo, 0};
	ASSERT_EQ(p.get_ptr(), &foo);
	ASSERT_EQ(p.get_state(), 0ul);
}

TEST(StatePointer, SetState) {
	Foo foo;
	state_ptr<Foo> p{&foo, 1};
	ASSERT_EQ(p.get_ptr(), &foo);
	ASSERT_EQ(p.get_state(), 1ul);
	p.set_state(2);
	ASSERT_EQ(p.get_ptr(), &foo);
	ASSERT_EQ(p.get_state(), 2ul);
}

TEST(StatePointer, EnumState) {
	Foo foo;
	state_ptr<Foo, Bar> p{&foo, Bar::A};
	ASSERT_EQ(p.get_ptr(), &foo);
	ASSERT_EQ(p.get_state(), Bar::A);
	p.set_state(Bar::B);
	ASSERT_EQ(p.get_ptr(), &foo);
	ASSERT_EQ(p.get_state(), Bar::B);
}

} // namspace
