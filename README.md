# state_ptr
A C++ pointer type with storage space for information filling.

Because of data alignment pointers that are used to iterate over arrays of data have states that cannot be reached.
For example for arrays of element type `uint64_t` the three least significant bits of the pointer are always the same (zero).

This class provides a convenient way to use this otherwise wasted space to store information.

## Release Notes

### 0.2.0

- Added comprehensive doxygen-ish documentation to all public parts of the library.
- Improved the internal representation of the pointer and the state that no longer is based on bitfields.
- It is now possible to use a user-defined type as state representable.
- Users can (and should) set a state value limit on `state_ptr` instances.
- Added constructors and comparisons for `nullptr_t`.
- Added bool conversion overload.
- Moved `state_ptr.hpp` from root directory to `include/putl/state_ptr.hpp` for more convenience.
- Lots of code refactoring and clean-up.
- Devel
	- Made the project a proper CMake project.
	- Added test suite for `state_ptr` and `log2`.

### 0.1.0

- Initial release.

## Example

```c++
using namespace putl;

struct Foo{ int baz; };
enum class Bar{ A, B, C };

Foo foo{42};
state_ptr<Foo, Bar> p{&foo, Bar::A};

// Stats for an exemplary 64-bit architecture
// ------------------------------------------
static_assert(state_ptr<Foo, Bar>::ptr_bits   == 61); // 61 bits for the pointer
static_assert(state_ptr<Foo, Bar>::state_bits == 3 ); // 3 bits for the state
static_assert(state_ptr<Foo, Bar>::threshold  == 7 ); // this is the greatest number that can be stored in the pointer's state

assert(p.get_state() == Bar::A);
assert(p.get_ptr()   == std::address_of(foo));

// Set state of `p` to `Bar::B`
// ----------------------------

p.set_state(Bar::B);
assert(p.get_state(), Bar::B);

// Use the pointed-to value
// ------------------------

foo->baz = 5; // sets foo's baz to `5`
assert(*foo == 5);

// Try setting the state of `p` to a value that is out-of-bounds results in a panic
// --------------------------------------------------------------------------------

p.set_state(static_cast<Bar>(42)); /* !! panic !! */
```

I hope this small pointer utility library is useful to you.
