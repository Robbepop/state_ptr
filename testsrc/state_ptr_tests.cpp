#include <gtest/gtest.h>

#include <putl/state_ptr.hpp>

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

TEST(StatePointer, SetStateOutOfBounds) {
	Foo foo;
	state_ptr<Foo> p{&foo, 0};
	ASSERT_DEATH(p.set_state(1337), "state value is out of bounds for this state_ptr");
}

} // namespace
