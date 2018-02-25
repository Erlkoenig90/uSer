# Introduction

µSer is a portable serialization library for C++17. Serialization is the process of turning data represented in data types of a programming language into a "flat" representation - typically, a sequence of bytes (8-Bit Integers - strictly, "Octets"). Deserialization is the opposite direction - bytes to data structures. This is necessary for transmitting data over a network or storing it into files. In other words, serialization is the answer to the question "How to convert an uint32_t to an uint8_t[4], and vice versa?".

There are many approaches and libraries to achieve this goal. Most of them focus on quickly and easily storing arbitrary data in a convenient format, such as XML, JSON or some specific binary representation. In contrast to that, µSer's main goal is to support given arbitrary binary formats in a platform-independent manner. Ideally, it should be possible to take a given binary format (e.g. defined by a standardized communication protocol or file format), write code based on µSer to handle the format once, and run it on any platform without modification, special compiler flags or language extensions. Basically, µSer provides a portable function to turn any data type into a uint8_t[] and back.

The key features are:
- µSer is implemented as a header-only metaprogramming library for C++17.
- No external tools, code generators, or specification languages are needed; everything is done by the C++ compiler.
- µSer is easy to integrate into existing codebases; ideally, the serialization functions serve as drop-in replacements for memcpy.
- The annotated structs are compatible with C, if no other C++ features are used. This makes it possible to define your data structures by a tree of structs in a header file that is shared between C and C++ code. Effectively, only very few lines of C++ codes are necessary to call µSer's functions. This makes it easy to integrate µSer into C projects.
- For the raw data, µSer supports the byte orders little endian, big endian and PDP endian. The byte order of the platform the application is running on has no influence on µSer's workings - in theory, µSer would continue to work on platforms with even different orders. The compiler's implementation of bitshifts should make sure that the bits end up in the right place.
- Signed integers can be represented in the raw data using Two's complement, One's complement and Signed-Magnitude. Again, the format of the host platform is irrelevant for µSer.
- µSer accepts data in the following types, known as serializable types:
	- All signed and unsigned integers
	- Classes containing serializable member variables, if annotated correctly
	- C-Arrays and std::array of serializable types
	- Containers that support iterators, including those of the C++ Standard Library
	- std::tuple's and std::pair's of serializable types
- The size of the binary representation of any object need not be a multiple of the byte size. µSer simulates bitfields in the binary data without actually using the C++-provided bitfields. This e.g. allows you to serialize one 11-Bit-Integer and 7 3-Bit-Integers into 3 Bytes ( = 32 bits).
- µSer makes no assumptions about the platform implementation especially regarding sizes of integer types, number of bits in a char (CHAR_BIT), sign formats, byte order, padding bytes, alignment requirements, overflow behaviour and aliasing, except as guaranteed by the C++17 standard. This allows you to use the same code on any platform and get the same result, or a compiler error if this is not possible (rather than random runtime failures).
- µSer achieves the desired data conversions by careful application of bitwise operations (mainly shifts and bitwise or). This avoids any reliance on implementation-defined behaviour and allows µSer to work consistently regardless of the presence of certain compiler options (especially regarding optimization, strict aliasing, or integer types), compiler version, processor architecture, or operating system.
- µSer is actually not limited to 8-bit-Bytes for the raw binary data - any unsigned integer type can be used to store the raw data. This underlying unsigned integer type will be called "serialization word", or short "SWord". This simplifies transmitting data over interfaces which accept e.g. 16 bits at once.
- The raw data can be stored in C-Arrays, std::array, or any other homogeneous container made up serialization words. It is also possible to provide arbitrary iterators for raw data input/output. This makes it possible to transmit/receive the raw data directly to/from any communication interface SWord by SWord.
- µSer has limited support for dynamic data structures, i.e. structures whose size is only known at runtime. The sizes of arrays can depend on previously (de)serialized data, as long as the size of the contained element is a multiple of the size of an SWord. Individual objects can be serialized or not depending on previous data as well, if their size is a multiple of the SWord size.
- µSer does not use dynamic memory management, and support for exceptions is optional. Whenever µSer uses a bit-shifting operator, the right-hand argument of ">>" or "<<" is always a compile-time known constant, making the bitshifts efficient even if the processor does not feature a barrel shifter. This makes it possible to use µSer on small embedded systems, hence its name.
- As an auxiliary functionality for testing purposes, µSer can print and compare serializable types.
- µSer is available under an open source license that also allows commercial usage, see below.

Limitations:
- There is currently no support for floating-point numbers.
- C++17 support is not widespread yet. As of this writing, only Clang and GCC support enough of C++17 to compile µSer. Since C++17 greatly simplifies metaprogramming and other compilers can be expected to catch up, older language standards are not supported by µSer.
- Not every conceivable format can be serialized by µSer.
- Compiling template-heavy code is slow; translating a code base with ~2000 individual calls to the serialization functions takes 7 minutes on an i3-4010U CPU. This can be worked around by placing the calls to the serialization functions in individual files that can be compiled in parallel and only need to be recompiled if the format changes.
- µSer makes no effort in resolving circles and redundant paths in object graphs - i.e. if some objects reference each other, µSer will enter an endless loop. If the same object is referenced twice, it will be serialized twice.
- Compilation errors related to metaprogramming are often hard to read. Where possible, µSer prints readable error messages via static_assert when an unsupported combination of data types and attributes was requested.
- Using the declaration macros, structs can have at maximum 364 members. If more are needed, they have to be annotated manually.

# License

Copyright (C) 2018 Niklas Gürtler

Contains a modified version of the [Map-Macro](https://github.com/swansontec/map-macro) Copyright (C) 2012 William Swanson

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the names of the authors or
their institutions shall not be used in advertising or otherwise to
promote the sale, use or other dealings in this Software without
prior written authorization from the authors.

# Getting started
To start using µSer, download the project's source code, add the contained "uSer" directory to the compiler's include path, or just copy the "uSer/uSer.hh" file into your project. Add an include directive to your C++ source code:
```c++
#include <uSer.hh>
```
Now you can start serializing data! A basic example is:
```c++
#include <cstdint>
#include <iostream>
#include <uSer.hh>

int main () {
	// Define an array to receive the raw binary data.
	std::uint8_t raw [4];
	// Define the data to be serialized.
	const std::uint32_t x = 0x54534554;
	// Perform the actual serialization.
	uSer::serialize (raw, x);
	// Write the binary data to standard output.
	std::cout.write (reinterpret_cast<char*> (raw), 4) << std::endl;
}
```
After defining a buffer for the raw data, and the data to be serialized, the serialize function is called to do the conversion. After that, the uint8_t array contains the values 0x54, 0x45, 0x53, 0x54. This order does _not_ depend on the byte order of the platform but is the little endian order, which is the default in µSer (can be changed). In other words, this result should be identical on all platforms, as long as they provide the types uint8_t and uint32_t. The four bytes are then written to the standard output. The four bytes just happen to be the ASCII encoding of the text "TEST", which is thereby output.

The opposite case of deserializing an array into an integer works as follows:
```c++
#include <cstdint>
#include <iostream>
#include <ios>
#include <uSer.hh>

int main () {
	// Define an array with binary data to be deserialized.
	const std::uint8_t raw [4] = { 0x54, 0x45, 0x53, 0x54 };
	// Define a variable that receives the deserialized data.
	std::uint32_t x;
	// Perform the actual deserialization.
	uSer::deserialize (raw, x);
	// Write the result to standard output.
	std::cout << "0x" << std::hex << x << std::endl;
}
```

# Documentation
The full documentation is included in the release packages (see link above) in doc/html/index.html. You can generate it from source by running "doxygen Doxyfile" within the "doc" directory. You can view it [online here](https://erlkoenig90.github.io/uSer-doc/html/index.html)
