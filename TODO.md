ToDo List for Pointer Utils
===========================

- Add a useful test suite via CMake.
- Implement CI via travisCI (Linux & macOS) and appveyor (Windows).
- Add comprehensive documentation for all API parts of the code.
- Introduce namespace specifier so that users of the library can change the default namespace.
- Implement `std::hash` for `state_ptr`. (Or only for `T`s that implement `std::hash` themselves?)
- Make the state of the `state_ptr` a template to allow for enums and other special types and better type safety on the user side.
