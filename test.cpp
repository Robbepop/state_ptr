#include "state_ptr.hpp"

#include <vector>
#include <iostream>

auto main() -> int {
	std::vector<int64_t> vec{5, -14, 28, 7, 7, 13, 0, 1, 3, -5, 97};

	putl::state_ptr<int64_t> p{&*vec.begin(), 0};
	putl::state_ptr<int64_t> q{&*vec.end()  , 0};

	std::cout << "state_ptr<int64_t>::state_bits = " << putl::state_ptr<int64_t>::state_bits << '\n';
	std::cout << "state_ptr<int64_t>::ptr_bits   = " << putl::state_ptr<int64_t>::ptr_bits << '\n';
	std::cout << "state_ptr<int64_t>::threshold  = " << putl::state_ptr<int64_t>::threshold << '\n';

	size_t cur_state = 0;
	for (auto r = p; r != q; ++r) {
		std::cout << "vec at " << r.get_ptr() << " = " << *r << '\t';
		cur_state = (cur_state + 1) % (putl::state_ptr<int64_t>::threshold + 1);
		p.set_state(cur_state);
		q.set_state(cur_state);
		std::cout << "p.get_state() = " << p.get_state() << '\n';
	}

	return 0;
}
