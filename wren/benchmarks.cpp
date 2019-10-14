#include <benchmark/benchmark.h>

#include "wren.hpp"

namespace {
bool loadfile(WrenVM* vm, const char* filename) {
    FILE *f;
    char buf[16384];

    f = fopen(filename, "rb");
    if (!f) {
		return false;
	}

	fread((void *) buf, 1, sizeof(buf), f);
	fclose(f);

	return wrenInterpret(vm, "main", buf) == WREN_RESULT_SUCCESS;
}

void write(WrenVM* vm, const char* s)
{
	(void) vm;
	printf("%s\n", s);
}

char g_lastError[2000] = { 0 };
void reportError(WrenVM* vm, WrenErrorType type, const char* module, int line, const char* message)
{
	(void)vm;
	switch (type)
	{
	case WREN_ERROR_COMPILE:
		sprintf(g_lastError, "compile error: %s:%d: %s\n", module, line, message);
		break;

	case WREN_ERROR_RUNTIME:
		sprintf(g_lastError, "runtime error: %s\n", message);
		break;

	case WREN_ERROR_STACK_TRACE:
		sprintf(g_lastError, "stack trace: %s:%d: %s\n", module, line, message);
		break;
	}
	fprintf(stderr, "%s\n", g_lastError);
}

void initConfig(WrenConfiguration* config) {
	wrenInitConfiguration(config);
	config->writeFn = write;
	config->errorFn = reportError;
	g_lastError[0] = '\0';
}
}

static void BM_CreateVM(benchmark::State& state) {
    for (auto _ : state) {
		WrenConfiguration config; 
		initConfig(&config);
		WrenVM* vm = wrenNewVM(&config);
		wrenFreeVM(vm);
	}
}
BENCHMARK(BM_CreateVM);

/*
// TODO: Wren does not seem to be a fan of redefining the same module in the same VM multiple times
static void BM_LoadNBody(benchmark::State& state) {
	WrenConfiguration config; 
	initConfig(&config);
	WrenVM* vm = wrenNewVM(&config);

	for (auto _ : state) {
		if (!loadfile(vm, "nbody.wren")) {
			state.SkipWithError(g_lastError);
			break;
		}

		wrenEnsureSlots(vm, 1);
		WrenHandle* callMain = wrenMakeCallHandle(vm, "main");
		benchmark::DoNotOptimize(callMain);

		wrenGetVariable(vm, "main", "Nope", 0);
		WrenHandle* SystemClass = wrenGetSlotHandle(vm, 0);
		benchmark::DoNotOptimize(SystemClass);
		wrenReleaseHandle(vm, callMain);
		wrenReleaseHandle(vm, SystemClass);
	}

	wrenFreeVM(vm);
}
BENCHMARK(BM_LoadNBody);
*/

static void BM_RunNBody(benchmark::State& state) {
	WrenConfiguration config; 
	initConfig(&config);
	WrenVM* vm = wrenNewVM(&config);

	if (!loadfile(vm, "nbody.wren")) {
		state.SkipWithError(g_lastError);
		wrenFreeVM(vm);
		for (auto _ : state) {}
		return;
	}

	wrenEnsureSlots(vm, 1);
    WrenHandle* callMain = wrenMakeCallHandle(vm, "main(_)");
	wrenGetVariable(vm, "main", "Nope", 0);
	WrenHandle* SystemClass = wrenGetSlotHandle(vm, 0);

	for (auto _ : state) {
		wrenEnsureSlots(vm, 2);
		wrenSetSlotHandle(vm, 0, SystemClass);
		wrenSetSlotDouble(vm, 1, static_cast<double>(state.range(0)));
		if (wrenCall(vm, callMain) != WREN_RESULT_SUCCESS) {
			state.SkipWithError(g_lastError);
			break;
		}
	}
	wrenReleaseHandle(vm, callMain);
	wrenReleaseHandle(vm, SystemClass);
	wrenFreeVM(vm);
}
BENCHMARK(BM_RunNBody)->Arg(1)->Arg(10)->Arg(100)->Unit(benchmark::kMillisecond);
