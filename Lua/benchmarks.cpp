#include <benchmark/benchmark.h>

#include "lua.hpp"

static void BM_Lua_CreateVM(benchmark::State& state) {
    for (auto _ : state) {
		lua_State *L = luaL_newstate();
		lua_close(L);
	}
}
BENCHMARK(BM_Lua_CreateVM);

static void BM_Lua_OpenLibs(benchmark::State& state) {
	lua_State *L = luaL_newstate();
    for (auto _ : state) {
		luaL_openlibs(L);
	}
	lua_close(L);
}
BENCHMARK(BM_Lua_OpenLibs);
