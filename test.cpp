#include "state_ptr.hpp"

#include <vector>
#include <iostream>

using namespace putl;

/// ===================================================================
///  Tests for the constexpr log2 implementation.
/// ===================================================================

static_assert(detail::log2(70) == 6, "");
static_assert(detail::log2(64) == 6, "");
static_assert(detail::log2(35) == 5, "");
static_assert(detail::log2(32) == 5, "");
static_assert(detail::log2(17) == 4, "");
static_assert(detail::log2(10) == 3, "");
static_assert(detail::log2( 4) == 2, "");
static_assert(detail::log2( 2) == 1, "");
static_assert(detail::log2( 1) == 0, "");
static_assert(detail::log2( 0) == 0, ""); // edge case

/// ===================================================================
///  Tests for the state_ptr implementation.
/// ===================================================================

static_assert(sizeof(state_ptr<uint32_t>) == sizeof(uint32_t*), "");
static_assert(state_ptr<uint32_t>::state_bits == 2, "");
static_assert(state_ptr<uint32_t>::ptr_bits == 8 * sizeof(uint32_t*) - state_ptr<uint32_t>::state_bits, "");

static_assert(sizeof(state_ptr<uint64_t>) == sizeof(uint64_t*), "");
static_assert(state_ptr<uint64_t>::state_bits == 3, "");
static_assert(state_ptr<uint64_t>::ptr_bits == 8 * sizeof(uint64_t*) - state_ptr<uint64_t>::state_bits, "");

auto main() -> int {
	std::vector<int64_t> vec{5, -14, 28, 7, 7, 13, 0, 1, 3, -5, 97};

	putl::state_ptr<int64_t> p{&*vec.begin(), 0};
	putl::state_ptr<int64_t> q{&*vec.end()  , 0};

	std::cout << "state_ptr<int64_t>::state_bits = " << putl::state_ptr<int64_t>::state_bits << '\n';
	std::cout << "state_ptr<int64_t>::ptr_bits   = " << putl::state_ptr<int64_t>::ptr_bits << '\n';
	std::cout << "state_ptr<int64_t>::threshold  = " << putl::state_ptr<int64_t>::threshold << '\n';

	// size_t cur_state = 0;
	// for (auto r = p; r != q; ++r) {
	// 	std::cout << "vec at " << r.get_ptr() << " = " << *r << '\t';
	// 	cur_state = (cur_state + 1) % (putl::state_ptr<int64_t>::threshold + 1);
	// 	p.set_state(cur_state);
	// 	q.set_state(cur_state);
	// 	std::cout << "p.get_state() = " << p.get_state() << '\n';
	// }

	return 0;
}
