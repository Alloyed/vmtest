#include <benchmark/benchmark.h>
/* !!! REMINDER: stack starts at 0 !!! */

#include "duktape.h"
#include "duk_console.h"

static void BM_CreateVM(benchmark::State& state) {
    for (auto _ : state) {
		duk_context* ctx = duk_create_heap_default();
		duk_destroy_heap(ctx);
	}
}
BENCHMARK(BM_CreateVM);

namespace {
int32_t loadfile(duk_context* ctx, const char* filename) {
    FILE *f;
    size_t len;
    char buf[16384];

    f = fopen(filename, "rb");
    if (!f) {
        duk_push_undefined(ctx);
		return 1; // error
	}

	len = fread((void *) buf, 1, sizeof(buf), f);
	fclose(f);

	int32_t dukflags = 0;
	duk_push_string(ctx, filename);
	return duk_pcompile_lstring_filename(ctx, dukflags, (const char *) buf, (duk_size_t) len);
}
}

static void BM_LoadNBody(benchmark::State& state) {
	duk_context* ctx = duk_create_heap_default();
    for (auto _ : state) {
		if (loadfile(ctx, "nbody.js") != 0) {
			state.SkipWithError(duk_safe_to_string(ctx, -1));
			duk_pop(ctx);
			break;
		}
		duk_pop(ctx);
	}
	duk_destroy_heap(ctx);
}
BENCHMARK(BM_LoadNBody);

static void BM_RunNBody(benchmark::State& state) {
	duk_context* ctx = duk_create_heap_default();
	duk_console_init(ctx, DUK_CONSOLE_PROXY_WRAPPER /*flags*/);

	if (loadfile(ctx, "nbody.js") != 0) { // [0]
		printf("loadfile failed\n");
		state.SkipWithError(duk_safe_to_string(ctx, -1));
		duk_pop(ctx);
	}
	else
	{
		// FIXME: the microbenchmark numbers on this are pretty sus
		for (auto _ : state) {
			duk_dup(ctx, 0); // [1] = nbody.js
			duk_push_int(ctx, state.range(0)); // [2]
			if(duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
				state.SkipWithError(duk_safe_to_string(ctx, -1));
				duk_pop(ctx); // [1]
				break;
			}
			duk_pop(ctx); // [1]
		}
	}
	duk_destroy_heap(ctx);
}
BENCHMARK(BM_RunNBody)->Arg(1)->Arg(10)->Arg(100)->Unit(benchmark::kMillisecond);
