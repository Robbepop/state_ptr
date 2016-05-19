# state_ptr
A C++ pointer type with storage space for information filling.

Because of data alignment pointers that are used to iterate over arrays of data have states that cannot be reached.
For example for arrays of element type `uint64_t` the three least significant bits of the pointer are always the same (zero).

This class provides a convenient way to use this otherwise wasted space to store information.

```c++
putl::state_ptr<int64_t> p = ...; // p.m_state == 0 by default
assert(p.get_state() == 0);
p.set_state(3); // checked at runtime if the given value is within bounds
assert(p.get_state() == 3);
p.set_state(8); // oops! not within bounds ...
```

I hope this is useful to you.
