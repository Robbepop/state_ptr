#include <gtest/gtest.h>

#include <putl/state_ptr.hpp>

namespace {

using namespace putl;

struct Foo{
	int a;
	auto get_a() const noexcept -> int { return a; }
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

TEST(StatePointer, ConstructStateOutOfBounds) {
	Foo foo;
	ASSERT_DEATH(state_ptr<Foo>(&foo, 1337), "state value is out of bounds for this state_ptr");
}

TEST(StatePointer, SetStateOutOfBounds) {
	Foo foo;
	state_ptr<Foo> p{&foo, 0};
	ASSERT_DEATH(p.set_state(1337), "state value is out of bounds for this state_ptr");
}

TEST(StatePointer, CopyConstructor) {
	Foo foo;
	state_ptr<Foo> p1{&foo, 1};
	state_ptr<Foo> p2{p1};
	ASSERT_EQ(p1, p2);
}

TEST(StatePointer, CopyAssignment) {
	Foo foo;
	state_ptr<Foo> p1{&foo, 1};
	state_ptr<Foo> p2{nullptr, 0};
	ASSERT_NE(p1, p2);
	p2 = p1;
	ASSERT_EQ(p1, p2);
}

TEST(StatePointer, EqualityReflexive) {
	Foo foo;
	state_ptr<Foo> p{&foo, 1};
	EXPECT_EQ(p, p);
}

TEST(StatePointer, EqualitySymmetry) {
	Foo foo;
	state_ptr<Foo> p1{&foo, 1};
	state_ptr<Foo> p2{&foo, 1};
	state_ptr<Foo> p3{nullptr, 2};

	EXPECT_EQ(p1, p2);
	EXPECT_EQ(p2, p1);

	EXPECT_NE(p1, p3);
	EXPECT_NE(p3, p1);

	EXPECT_NE(p2, p3);
	EXPECT_NE(p3, p2);
}

TEST(StatePointer, EqualityPtrAndState) {
	Foo foo1;
	Foo foo2;
	state_ptr<Foo> p11{&foo1, 1};
	state_ptr<Foo> p12{&foo1, 2};
	state_ptr<Foo> p21{&foo2, 1};
	state_ptr<Foo> p22{&foo2, 2};

	EXPECT_NE(p11, p12);
	EXPECT_NE(p11, p21);
	EXPECT_NE(p11, p22);
}

TEST(StatePointer, DerefOp) {
	int foo1{42};
	int foo2{1337};
	state_ptr<int> p1a{&foo1, 1};
	state_ptr<int> p1b{&foo1, 1};
	state_ptr<int> p2 {&foo2, 1};
	EXPECT_EQ(*p1a, *p1b);
	EXPECT_NE(*p1a, *p2);
	EXPECT_NE(*p1b, *p2);
}

TEST(StatePointer, ArrowOp) {
	Foo foo1{42};
	Foo foo2{1337};
	state_ptr<Foo> p1a{&foo1, 1};
	state_ptr<Foo> p1b{&foo1, 1};
	state_ptr<Foo> p2 {&foo2, 1};
	EXPECT_EQ(p1a->get_a(), p1b->get_a());
	EXPECT_NE(p1a->get_a(), p2->get_a());
	EXPECT_NE(p1b->get_a(), p2->get_a());
}

TEST(StatePointer, ImplicitBoolConversion) {
	int foo{42};
	state_ptr<int> p1{&foo, 1};
	state_ptr<int> p2{nullptr, 1};
	EXPECT_TRUE(p1);
	EXPECT_FALSE(p2);
}

TEST(StatePointer, RegressionGitHubIssue4) {
	// Compile-time regression where apple-clang reported the following error:
	// 
	// [...]/state_ptr/include/putl/state_ptr.hpp:263:10: error: reinterpret_cast from rvalue to reference type
	//       'reference_type' (aka 'int &')
	//                 return reinterpret_cast<reference_type>(get_ptr());
	//                        ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// [...]/state_ptr/testsrc/state_ptr_tests.cpp:119:18: note: in instantiation of member function
	//       'putl::state_ptr<int, unsigned long, 2>::operator*' requested here
	//     int target = *underTest;
	int testData = 1;
	putl::state_ptr<int> underTest{&testData, 0};
	int target = *underTest;
	ASSERT_EQ(target, 1);
}

} // namespace
