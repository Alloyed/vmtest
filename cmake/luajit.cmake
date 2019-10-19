include(ExternalProject)

ExternalProject_Add(project_luajit
	URL ${CMAKE_SOURCE_DIR}/deps/luajit # use URL to copy out-of-tree
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/luajit
	CONFIGURE_COMMAND ""
	BUILD_COMMAND make CFLAGS="-DLUAJIT_DISABLE_JIT"
	BUILD_IN_SOURCE 1
	INSTALL_COMMAND make install
	PREFIX=${CMAKE_CURRENT_BINARY_DIR}/luajit
	)
ExternalProject_Get_Property(project_luajit install_dir)
add_library(libluajit STATIC IMPORTED)
set_target_properties(libluajit PROPERTIES
	IMPORTED_LOCATION ${install_dir}/lib/libluajit-5.1.a
	INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/deps/lua-5.1.5/src"
)
target_link_libraries(libluajit INTERFACE dl)
add_dependencies(libluajit project_luajit)
