# state_ptr
A C++ pointer type with storage space for information filling.

Because of data alignment pointers that are used to iterate over arrays of data have states that cannot be reached.
For example for arrays of element type `uint64_t` the three least significant bits of the pointer are always the same (zero).

This class provides a convenient way to use this otherwise wasted space to store information.

```c++
std::array<int64_t, 4> arr{124, -97, 5, 77};
putl::state_ptr<int64_t> p{&arr[0], 0};
static_assert(putl::state_ptr<int64_t>::state_bits == 3, "");
static_assert(putl::state_ptr<int64_t>::ptr_bits == 61, ""); // on 64-bit system
static_assert(putl::state_ptr<int64_t>::threshold == 7, ""); // this is the greater number that can be stored in the pointer's state
std::cout << "*p = " << *p; // prints: "*p = 124"
p++;
std::cout << "*p = " << *p; // prints: "*p = -97"
assert(p.get_state() == 0);
p.set_state(3); // checked at runtime if the given value is within bounds
assert(p.get_state() == 3);
p.set_state(8); // oops! not within bounds ...
```

I hope this is useful to you.
