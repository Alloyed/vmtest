# the process behind getting and compiling a v8 binary is very complicated :(
# I just copied the final artifacts into the deps/ directory
add_library(libv8 STATIC IMPORTED)
set_target_properties(libv8 PROPERTIES
	IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/deps/v8/obj/libv8_monolith.a
	INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/deps/v8/include"
)
target_link_libraries(libv8 INTERFACE pthread)
