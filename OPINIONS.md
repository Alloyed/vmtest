Lua 5.1/5.3
===========
We all know what this is
* build system integration: Pretty simple, just make a static library that consists of all the .c/.h files in src/
* API. known quantity. Stack based API's are easy to mess up when hand writing code, but are nice for codegen/template metaprogramming situations
* debugger: https://github.com/slembcke/debugger.lua
* debugger: https://marketplace.visualstudio.com/items?itemName=devCAT.lua-debug, requires patch

Luajit
===========
Like lua.
* build system integration: The docs insist you stick to the makefile. cmake has a slightly arcane way of supporting this but I half-suspect some people may want to to build their own scripts anyways and it's disappointing that there's no notes on this. likewise, the DISABLE_JIT flag i only found through external resources, not the docs themselves.
* API: same as lua.
* debugger: same, nobody has made an OP_HALT patch tho :(

Duktape
===========
Javascript, but with a focus on embedding.
* build system integration: the easiest of all. an amalgamated build is provided, which means you could even build it as a header-only library with minimal edits.
* API: Stack based API, like lua. many concepts seem to transfer over. There are some extra complexities around the concepts that are themselves complex in 
* debugger: https://marketplace.visualstudio.com/items?itemName=koush.duk-debug

V8
===========
I can't get it to install :x

Wren
===========
Fresh new language with fresh new problems!
* build system integration: same as lua, just build all files in the src/ folder
* API: Wren's API is still nominally stack based, but it does way more with "handles", which are created resources that the C++ side is meant to hold onto and are meant to "cache" the process of calling a function or accessing a variable. Real applications using wren will probably want to adopt a strict ownership model around handles, as they need to be freed before the VM itself can be freed.
* I'm not the biggest fan of the error handling model, which will return a single error code and pass all the actual error info through an alternate callback, line-by-line. This approach seems to fit printing to stderr well and not much else :/
* debugger: none
