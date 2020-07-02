# A direct comparison between lua/wren from a C++ embedders perspective I guess

## none of lua's vaguely offputting design decisions!

Index by 0! lists and maps are different things! Assignment isn't declaration!
You can't accidentally call a static method as an instance method and
viceversa!

Ok, pulling back the curtain a bit; I don't think those things are actually
dealbreakers, and I don't think they should be dealbreakers for your purposes,
either. If I'm being totally honestly this whole list is kinda whingey in about
the same way. With a few exceptions, none of this stuff is a dealbreaker for
wren either, and none of them seem like fatal, unfixable flaws.

## handles

A concept unique to wren is the "handle". This is basically a pointer you get
to keep in C++ to a piece of wren-managed memory; there is a call to acquire a
handle and an equivalent call to free a handle that I assume adds and removes
reference counts inside of the VM.

For the kinds of problems that handles solve, lua instead uses something called
the registry. the registry is a normal table, only accessible from C++ by
default, and you can store whatever you want in it, same as any other table.
there are convenience methods to pick a good unique key to put stuff into the
registry, and you can keep that key around to pull it out later.

If I were to describe the memory model of each of these, handles are "strong"
references while registry keys are "weak" references. While you can usually
assume that nobody will unref your registry objects; the API is built to assume
that it is possible, and a safety-oriented programmer will at least assert to
ensure that you actually got the thing you expected out of the registry. By
contrast, so long as you manage your memory properly (big if!), a handle will
always be a valid reference.

In either case: creating extra guard rails, in the form of a C++-side RAII
wrapper/reference count or similar, is probably a good idea.

(minor note: one thing I'd like to see are handles to string memory, too~.
string comparisons are pretty common and if I could just get a handle/hash
value and compare ints instead of strings that'd be very handy.)

## slots

Instead of a "stack", wren uses "slots". To be perfectly honest, these are
basically equivalent. the stack is a place you can temporarily track references
to lua values, and manipulate/get/set them. Slots are a place you can
temporarily track references to wren values, and manipulate/get/set them. The
difference is mostly in ergonomics/design intent: lua's built in functions will
often refer to stack indices relative to the "top" of the stack, or manipulate
and push things on and off of the top. Wren's slot array uses explicit,
absolute indices with a few exceptions. You can think of wren as indexing
things from the "bottom" as opposed to the top.

Wren's approach is easier to read for most people, and relies less on the
implicit contract between methods and the stack. for example, `lua_settable(L,
tableIndex)` will set a key-value pair in the specified table, but the key
_must_ be at top-1 and the value _must_ be at top, and both values will be
removed after they are used. None of this is obvious from looking at the call
itself, you need to refer to documentation to know that it's happening. By
contrast, `wrenInsertInList(vm, listSlot, indexSlot, valueSlot)` tells you
everything in advance, and doesn't implicitly manipulate anything.

This makes writing generic helper methods a bit more tricky in wren, though.
You can't just push things onto the "top" for your working values; there's not
even a top to speak of. you can emulate it via manual parameter passing; eg
`mymethod(vm, outputSlot1, outputSlot2, firstWorkingSlot)` can be written to
return two wren values, and use all the slots above firstWorkingSlot to get the
job done. In lua this could all be done implicitly; the caller would not even
need to know that the method needed working slots, you could just push them/pop them
as necessary.

## No re-entrancy

https://github.com/wren-lang/wren/issues/487

What this means, in effect, is that you cannot call a wren function/method from
inside a wren function. How does this restrict you? It means, for example, that
you cannot write foreign methods that can treat functions as first class,
because they can't call them. A foreign class that wraps over a C++ list could
not implement many common higher order functions like map, reduce, etc. in C++.
You cannot access data from inside wren objects from C++, because getters are
methods. etc etc

## No varargs

Wren uses a novel dispatch approach I'm going to call dispatch-on-arity. you
can have multiple methods defined for a single symbol, so long as they take
different numbers of arguments. Variable length arguments are left out,
although I don't know if it's a design decision or not. You are also
technically limited to 16 arguments, which I can both say I've gone past in
real production code and also I don't care that it's a limit.

## Incremental reload

Wren does not currently have a concept of incremental reload. When a class is
defined, it's done. if you want to replace that class/module your best bet is
to torch the entire VM and start over.

Lua by comparison will let you do what you want when you want it; it may not
have an explicit concept of a reload but its hashmap based design makes this
very easy to do on your own. The simplest approach to incremental reload is to
just run the file again: anything defined within that file will just
re-defined. With a little bit of clever library design you can also get
reloading classes and class instances for very little work, too.

In practice this won't really matter until wren programs start getting large,
or it starts getting embedded into projects that are already large in
scope/dataset: reloading your entire wren universe probably doesn't take more
than a few frames of game time away, so you can get away with doing that pretty
often. This gets more complex and expensive the more parallel structures you
make from inside wren: if for example in your game engine deleting the VM means
deleting images/sound assets that the VM was holding onto, loading those a
second time could be very expensive, and likewise if you need to attach a lot
of wren callbacks or whatever to an existing entity structure.

## API safety

This one sort of scares me the most; the general attitude of wren seems to be
that API validation/bounds checks should be opt-in, if they even exist at all.
This to me seems to be counter to the point of using a scripting language in
the first place; if I were ok with segfaulting and having to restart my
application I could very easily just do that from C++ directly.

## The Vec2/Vec3/Vec4 problem

If this seems like a personal bugbear, that's because it is :) but I do believe
that this is geniunely important. Very often in games, you have data that can
be represented as multiple numbers across difference axes. 2d! 3d! Quaternions
and colors! The only tool we have right now to express those in wren are
Objects, which are fine and good but put a limit to how many you can use before
you start to have garbage collection problems.

In C++ you can write
```cpp
	void Object::MyMethod(const Vec2 myvec)
	{
		m_internalVector += myvec;
	}
```
which will prompt the compiler to stack allocate the vec2; in place modify the
internal vector, and in general output the same machine instructions that you
would have if you had instead used 2 independent variables for X and Y.

In wren the equivalent code:
```wren
class Vec {
	...
	+(other) { return Vec.new(_x + other.x, _y + other.y) }
}
class Object {
	myMethod(myvec) {
		_internalVector = _internalVector + myvec;
	}
}
```
means you actually allocate a new instance of `_internalVector`, and that you
push the old instance to the garbage heap. Add that up over a complex game and
it starts to be the _main_ source of garbage churn, and even complex solutions
like vector object pooling still don't work as good as not needing the vector
in the first place. Lua users will often just use the two/three/four variables
that a vector makes up, which avoids the performance cost but makes dealing
with any kind of vector math a pain.

One solution is to upgrade Vec2/Vec3/Vec4 into primitive data types. This does
not play well with nan-tagging, for obvious size reasons (can't fit four
doubles into the space of one!) but you may be able to make an object pool
that's built into the VM instead and that may work better? Generalized, you
could add a dedicated tuple type, as python has; I don't know the performance
implications of this. You also run into the limitations of no monkey-patching
here. I wouldn't expect a standard library to provide every useful math
operation for a vec type, or to meaningfully figure out how each vec would be
used; I just want the storage benefits honestly.

side note: you'll notice I left out matrixes, which if you want to get crazy
with it you could technically consider a Vec16. I wouldn't realistically expect
a C++ compiler to efficiently copy that so treating those as Objects makes
sense, still. This does mean that you can't _really_ use operator overloading
with them unless there are dedicated inplace operators or a way to specify temp
storage.

## Error handling/Debug API.

I can chalk this one up to language immaturity; but the main way errors and
aborts are handled in wren is to register a callback that recieves a traceback
as a fully-formed string. lua by comparison gives you accessors for every piece
of information in a traceback in the form of debug.getinfo(), and it can be
called even outside of an exception.

One thing that lua doesn't have by default, but all languages really should, is
a way to halt or inject code at specific points of excecution. There's a patch
floating around for lua called the OP_HALT patch, which implements this by
creating a new bytecode instruction (OP_HALT) and defining ways to add and
remove it from already compiled code at runtime. Between this patch and a debug
introspection API, that's pretty much all you need from an interpreter
implementation to get a real(tm) breakpoint debugger.

there is also a sense that I get that wren doesn't have a 100% answer for how
non-exceptional errors should be handled and reasonably be reported/bubbled up
and down the callstack. This is a point of active iteration even for the newest
languages with result types and pattern matching and all that jazz, but wren is
a new language too! and the problems still exist, even in dynamic languages.
