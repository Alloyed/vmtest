#include <benchmark/benchmark.h>

#include "duktape.h"

static void BM_CreateVM(benchmark::State& state) {
    for (auto _ : state) {
		duk_context* ctx = duk_create_heap_default();
		duk_destroy_heap(ctx);
	}
}
BENCHMARK(BM_CreateVM);
