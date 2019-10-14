project(duktape C)

add_library(libduktape src/duktape.c)
set_target_properties(libduktape PROPERTIES
	OUTPUT_NAME duktape
	INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
