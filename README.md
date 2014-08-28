xchange
=======

This is a simple project that build a bridge between:

* [msgpack-c](https://github.com/msgpack/msgpack-c)
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
* [RapidJSON](https://github.com/open-source-parsers/jsoncpp).

Source Code
-----------

The `src/main.cpp` is a demo usage.
The jsoncpp adapter for msgpack-c is `src/msgpack/type/jsoncpp/value.hpp`.
The RapidJSON adapter for msgpack-c is `src/msgpack/type/rapidjson/document.hpp`.

Usage
-----

Just check out the `src/main.cpp`.

*Maybe I will document usage here later.*

Current status
--------------

From \ To      | msgpack-c   | jsoncpp             | RapidJSON
---------------|-------------|---------------------|------------
**msgpack-c**  | \-          | Done                | Blocked
**jsoncpp**    | Done        | \-                  | Blocked
**RapidJSON**  | Done        | Done(via msgpack-c) | \-

*The convert to RapidJSON is blocked because the `rapidjson::Value.AddMember()` [requires an allocator](https://github.com/xpol/xchange/blob/master/src/msgpack/type/rapidjson/document.hpp#L42)
which is neither supported by msgpack-c nor RapidJSON.*

