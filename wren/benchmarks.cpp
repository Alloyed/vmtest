#include <benchmark/benchmark.h>

#include "wren.hpp"

static void BM_CreateVM(benchmark::State& state) {
    for (auto _ : state) {
		WrenConfiguration config; 
		wrenInitConfiguration(&config);
		WrenVM* vm = wrenNewVM(&config);
		wrenFreeVM(vm);
	}
}
BENCHMARK(BM_CreateVM);

