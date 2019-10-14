#include <benchmark/benchmark.h>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

static void BM_CreateVM(benchmark::State& state) {
    for (auto _ : state) {
		lua_State *L = luaL_newstate();
		luaL_openlibs(L);
		lua_close(L);
	}
}
BENCHMARK(BM_CreateVM);

static void BM_LoadNBody(benchmark::State& state) {
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
    for (auto _ : state) {
		luaL_loadfile(L, "nbody.lua"); // [1]
		if (!lua_isfunction(L, 1)) {
			state.SkipWithError(lua_tostring(L, 1));
			break;
		}
	}
	lua_close(L);
}
BENCHMARK(BM_LoadNBody);

static void BM_RunNBody(benchmark::State& state) {
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	luaL_loadfile(L, "nbody.lua"); // [1]
	if (!lua_isfunction(L, 1)) {
		state.SkipWithError(lua_tostring(L, 1));
		lua_close(L);
		return;
	}
    for (auto _ : state) {
			lua_pushvalue(L, 1); // [2]
			lua_pushinteger(L, state.range(0)); // [3]
			if (lua_pcall(L, 1, 0, 0) != 0) { // [2]([3]) -> [2]
				state.SkipWithError(lua_tostring(L, -1));
				break;
			}
	}
	lua_close(L);
}
BENCHMARK(BM_RunNBody)->Arg(1)->Arg(10)->Arg(100)->Unit(benchmark::kMillisecond);
