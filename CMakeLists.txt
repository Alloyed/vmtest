cmake_minimum_required(VERSION 3.10)

project(vmtest)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${vmtest_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_BUILD_TYPE Debug)

include(cotire)

add_subdirectory(deps/fmt)

add_executable(vmtest
	Main.cpp
	Util/Log.cpp
)

target_include_directories(vmtest SYSTEM PRIVATE
	$ENV{INCLUDE}
	"${vmtest_SOURCE_DIR}/deps/include"
	"${vmtest_SOURCE_DIR}/deps/fmt/include"
)

target_compile_definitions(vmtest PRIVATE)
target_include_directories(vmtest PUBLIC ${vmtest_SOURCE_DIR})

target_link_libraries(vmtest fmt::fmt)


if(MSVC)
	target_compile_options(vmtest PRIVATE /W4 /WX)
else()
	target_compile_options(vmtest PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

target_compile_definitions(vmtest PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

cotire(vmtest)
