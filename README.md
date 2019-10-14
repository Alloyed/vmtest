Embedded language benchmarks
============================

Suite of benchmarks for testing a language implementation's
suitability/performance when embedded in a larger C++ application.

Requirements:
* loads/runs code at runtime
* No/limited global state (this means no GIL, specifically)
* No JIT compilation. If the language has a flag to disable all JIT, that's fine.
* A well defined C/C++ api

Evaluation criteria:
* Raw language performance
* C++ API performance (this includes marshalling and so on)
* Startup times on a small codebase
* Startup times on a large codebase (will need some sort of codegen to simulate this)
* Memory usage

Non-performance criteria I hope to suss out:
* Ease of embedding
* Ability to write complex native-feeling binding layers
* Ability to generate binding layers via codegen

Things I'm interested in evaluating:
- [ ] duktape
- [ ] v8
- [x] PUC lua 5.1
- [x] PUC lua 5.3
- [ ] luajit
- [x] wren
- [ ] dart
- [ ] haxe

Running Tests
-------------

We're using CMake as our build system.
All dependencies are bundled with the application.
```sh
$ git submodule update --init --recursive
$ mkdir build
$ cd build
build $ cmake ..
build $ make
build $ ctest --verbose
```

