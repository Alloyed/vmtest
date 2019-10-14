cmake_minimum_required(VERSION 3.10)

project(vmtest)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${vmtest_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_BUILD_TYPE Debug)

enable_testing()

#include(cotire)

# languages
add_subdirectory(deps/lua-5.1.5) # liblua51
add_subdirectory(deps/lua-5.3.5) # liblua53
add_subdirectory(deps/duktape-2.4.0) # libduktape

# harnesses
#add_subdirectory(deps/googletest)
SET(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "X" FORCE)
SET(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "X" FORCE)
SET(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "X" FORCE)
add_subdirectory(deps/benchmark)

# shared config
if(MSVC)
	add_compile_options(/W4 /WX)
else()
	add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_compile_definitions("$<$<CONFIG:DEBUG>:DEBUG>")

# executables
add_executable(test-lua51
	lua/benchmarks.cpp
)
target_include_directories(test-lua51 SYSTEM PRIVATE "${LUA_INCLUDE}")
target_link_libraries(test-lua51 liblua51 benchmark_main)
add_test(lua51 test-lua51)

add_executable(test-lua53
	lua/benchmarks.cpp
)
target_include_directories(test-lua53 SYSTEM PRIVATE "${LUA_INCLUDE}")
target_link_libraries(test-lua53 liblua53 benchmark_main)
add_test(lua53 test-lua53)

add_executable(test-duktape
	js/duktape-benchmarks.cpp
)
message("Include: ${DUKTAPE_INCLUDE}")
target_include_directories(test-duktape SYSTEM PRIVATE "${DUKTAPE_INCLUDE}")
target_link_libraries(test-duktape libduktape benchmark_main)
add_test(duktape test-duktape)

#cotire(vmtest)
