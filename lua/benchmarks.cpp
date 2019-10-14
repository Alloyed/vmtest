#include <benchmark/benchmark.h>

#include "lua.hpp"

static void BM_CreateVM(benchmark::State& state) {
    for (auto _ : state) {
		lua_State *L = luaL_newstate();
		luaL_openlibs(L);
		lua_close(L);
	}
}
BENCHMARK(BM_CreateVM);
