// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include <benchmark/benchmark.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libplatform/libplatform.h"
#include "v8.h"

namespace {
	// Extracts a C string from a V8 Utf8Value.
	const char* ToCString(const v8::String::Utf8Value& value) {
	  return *value ? *value : "<string conversion failed>";
	}

	void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch) {
		v8::HandleScope handle_scope(isolate);
		v8::String::Utf8Value exception(isolate, try_catch->Exception());
		const char* exception_string = ToCString(exception);
		v8::Local<v8::Message> message = try_catch->Message();
		if (message.IsEmpty()) {
			// V8 didn't provide any extra information about this error; just
			// print the exception.
			fprintf(stderr, "%s\n", exception_string);
		} else {
			// Print (filename):(line number): (message).
			v8::String::Utf8Value filename(isolate,
					message->GetScriptOrigin().ResourceName());
			v8::Local<v8::Context> context(isolate->GetCurrentContext());
			const char* filename_string = ToCString(filename);
			int linenum = message->GetLineNumber(context).FromJust();
			fprintf(stderr, "%s:%i: %s\n", filename_string, linenum, exception_string);
			// Print line of source code.
			v8::String::Utf8Value sourceline(
					isolate, message->GetSourceLine(context).ToLocalChecked());
			const char* sourceline_string = ToCString(sourceline);
			fprintf(stderr, "%s\n", sourceline_string);
			// Print wavy underline (GetUnderline is deprecated).
			int start = message->GetStartColumn(context).FromJust();
			for (int i = 0; i < start; i++) {
				fprintf(stderr, " ");
			}
			int end = message->GetEndColumn(context).FromJust();
			for (int i = start; i < end; i++) {
				fprintf(stderr, "^");
			}
			fprintf(stderr, "\n");
			v8::Local<v8::Value> stack_trace_string;
			if (try_catch->StackTrace(context).ToLocal(&stack_trace_string) &&
					stack_trace_string->IsString() &&
					v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
				v8::String::Utf8Value stack_trace(isolate, stack_trace_string);
				const char* stack_trace_string = ToCString(stack_trace);
				fprintf(stderr, "%s\n", stack_trace_string);
			}
		}
	}

	v8::MaybeLocal<v8::String> ReadFile(v8::Isolate* isolate, const char* name) {
		FILE* file = fopen(name, "rb");
		if (file == NULL) return v8::MaybeLocal<v8::String>();

		fseek(file, 0, SEEK_END);
		size_t size = ftell(file);
		rewind(file);

		char* chars = new char[size + 1];
		chars[size] = '\0';
		for (size_t i = 0; i < size;) {
			i += fread(&chars[i], 1, size - i, file);
			if (ferror(file)) {
				fclose(file);
				return v8::MaybeLocal<v8::String>();
			}
		}
		fclose(file);
		printf("file: %s", chars);
		v8::MaybeLocal<v8::String> result = v8::String::NewFromUtf8(
				isolate, chars, v8::NewStringType::kNormal, static_cast<int>(size));
		delete[] chars;
		return result;
	}

	// Executes a string within the current v8 context.
	bool ExecuteString(v8::Isolate* isolate, v8::Local<v8::String> source,
			v8::Local<v8::Value> name, bool print_result = false) {
		v8::HandleScope handle_scope(isolate);
		v8::ScriptOrigin origin(name);
		v8::Local<v8::Context> context(isolate->GetCurrentContext());
		v8::Local<v8::Script> script;
		if (!v8::Script::Compile(context, source, &origin).ToLocal(&script)) {
			return false;
		} else {
			v8::Local<v8::Value> result;
			if (!script->Run(context).ToLocal(&result)) {
				return false;
			} else {
				if (print_result && !result->IsUndefined()) {
					// If all went well and the result wasn't undefined then print
					// the returned value.
					v8::String::Utf8Value str(isolate, result);
					const char* cstr = ToCString(str);
					printf("%s\n", cstr);
				}
				return true;
			}
		}
	}
	
	// Executes a string within the current v8 context.
	bool ExecuteFile(v8::Isolate* isolate, const char* filename,
			bool print_result = false) {
		v8::TryCatch try_catch(isolate);
		v8::HandleScope handle_scope(isolate);

		v8::MaybeLocal<v8::String> maybeSrc = ReadFile(isolate, filename);
		v8::MaybeLocal<v8::String> maybeName = v8::String::NewFromUtf8(isolate, filename);

		v8::Local<v8::String> src;
		v8::Local<v8::String> name;
		if (!maybeSrc.ToLocal(&src) || !maybeName.ToLocal(&name)) {
			return false;
		}

		return ExecuteString(isolate, src, name, print_result);
	}
}

static void BM_CreateVM(benchmark::State& state) {
	auto* alloc = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    for (auto _ : state) {
		v8::Isolate::CreateParams create_params;
		create_params.array_buffer_allocator = alloc;
		v8::Isolate* isolate = v8::Isolate::New(create_params);
		isolate->Dispose();
	}
	delete alloc;
}
BENCHMARK(BM_CreateVM);

static void BM_LoadNBody(benchmark::State& state) {
	//
	auto* alloc = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	v8::Isolate::CreateParams create_params;
	create_params.array_buffer_allocator = alloc;
	v8::Isolate* isolate = v8::Isolate::New(create_params);
    for (auto _ : state) {
		v8::TryCatch try_catch(isolate);
		if (!ExecuteFile(isolate, "nbody.js")) {
			ReportException(isolate, &try_catch);
			state.SkipWithError("execute error!");
			break;
		}
	}
	isolate->Dispose();
	delete alloc;
}
BENCHMARK(BM_LoadNBody);

int main(int argc, char** argv) {
	// Initialize V8.
	v8::V8::InitializeICUDefaultLocation(argv[0]);
	v8::V8::InitializeExternalStartupData(argv[0]);
	std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
	v8::V8::InitializePlatform(platform.get());
	v8::V8::Initialize();

	::benchmark::Initialize(&argc, argv);
	if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
	::benchmark::RunSpecifiedBenchmarks();

	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
}
