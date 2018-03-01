\mainpage µSer Serialization Library

\section Introduction
This is the documentation for the µSer portable serialization library for C++17. Serialization is the process of turning data represented in data types of a programming language into a "flat" representation - typically, a sequence of bytes (8-Bit Integers - strictly, "Octets"). Deserialization is the opposite direction - bytes to data structures. This is necessary for transmitting data over a network or storing it into files. In other words, serialization is the answer to the question "How to convert an uint32_t to an uint8_t[4], and vice versa?".

There are many approaches and libraries to achieve this goal. Most of them focus on quickly and easily storing arbitrary data in a convenient format, such as XML, JSON or some specific binary representation. In contrast to that, µSer's main goal is to support given arbitrary binary formats in a platform-independent manner. Ideally, it should be possible to take a given binary format (e.g. defined by a standardized communication protocol or file format), write code based on µSer to handle the format once, and run it on any platform without modification, special compiler flags or language extensions. Basically, µSer provides a portable function to turn any data type into a uint8_t[] and back.

The key features are:
- µSer is implemented as a header-only metaprogramming library for C++17.
- No external tools, code generators, or specification languages are needed; everything is done by the C++ compiler.
- µSer is easy to integrate into existing codebases; ideally, the serialization functions serve as drop-in replacements for memcpy.
- The annotated structs are \ref CCompat "compatible with C", if no other C++ features are used. This makes it possible to define your data structures by a tree of structs in a header file that is shared between C and C++ code. Effectively, only very few lines of C++ codes are necessary to call µSer's functions. This makes it easy to integrate µSer into C projects.
- For the raw data, µSer supports the byte orders \ref uSer::ByteOrder::LE "little endian", \ref uSer::ByteOrder::BE "big endian" and \ref uSer::ByteOrder::PDP "PDP endian". The byte order of the platform the application is running on has no influence on µSer's workings - in theory, µSer would continue to work on platforms with even different orders. The compiler's implementation of bitshifts should make sure that the bits end up in the right place.
- Signed integers can be represented in the raw data using \ref uSer::SignFormat::TwosComplement "Two's complement", \ref uSer::SignFormat::OnesComplement "One's complement" and \ref uSer::SignFormat::SignedMagnitude "Signed-Magnitude". Again, the format of the host platform is irrelevant for µSer.
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
- As an auxiliary functionality for testing purposes, µSer can \ref uSer::print "print" and \ref uSer::compare "compare" serializable types.
- µSer is available under an open source license that also allows commercial usage, see below.

Limitations:
- There is currently no support for floating-point numbers.
- C++17 support is not widespread yet. As of this writing, only Clang and GCC support enough of C++17 to compile µSer. Since C++17 greatly simplifies metaprogramming and other compilers can be expected to catch up, older language standards are not supported by µSer.
- Not every conceivable format can be serialized by µSer.
- Compiling template-heavy code is slow; translating a code base with ~2000 individual calls to the serialization functions takes 7 minutes on an i3-4010U CPU. This can be worked around by placing the calls to the serialization functions in individual files that can be compiled in parallel and only need to be recompiled if the format changes.
- µSer makes no effort in resolving circles and redundant paths in object graphs - i.e. if some objects reference each other, µSer will enter an endless loop. If the same object is referenced twice, it will be serialized twice.
- Compilation errors related to metaprogramming are often hard to read. Where possible, µSer prints readable error messages via static_assert when an unsupported combination of data types and attributes was requested.
- Using the declaration macros, structs can have at maximum 364 members. If more are needed, they have to be annotated manually.

\section License

Copyright (C) 2018 Niklas Gürtler<br/>
Contains a modified version of the [Map-Macro](https://github.com/swansontec/map-macro) Copyright (C) 2012 William Swanson<br/>
<br/>
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

\section GettingStarted Getting started
To start using µSer, download the project's source code, add the contained "uSer" directory to the compiler's include path, or just copy the \ref uSer.hh "uSer/uSer.hh" file into your project. Switch on your compiler's C++17 support, e.g. by passing "-std=c++17" to the command line for GCC and Clang. Add an include directive to your C++ source code:
\code{.cpp}
#include <uSer.hh>
\endcode
Now you can start serializing data! A basic example is:
\include 01_basic_write.cpp
After defining a buffer for the raw data, and the data to be serialized, the \ref uSer::serialize serialize function is called to do the conversion. After that, the uint8_t array contains the values 0x54, 0x45, 0x53, 0x54. This order does _not_ depend on the byte order of the platform but is the \ref uSer::ByteOrder::LE "little endian" order, which is the default in µSer (can be changed). In other words, this result should be identical on all platforms, as long as they provide the types uint8_t and uint32_t. The four bytes are then written to the standard output. The four bytes just happen to be the ASCII encoding of the text "TEST", which is thereby output.

The opposite case of deserializing an array into an integer works as follows:

\include 02_basic_read.cpp

See \ref CCompat for how to use µSer in C projects.

\section Testcases
µSer ships with a set of unit tests. To run them, call "make" inside the "test" directory. If you only want to run the Clang tests, run "make run-clang". For g++, run "make run-gcc". Compiling the test cases takes a long time, but be careful with "-j" because it might eat up all your RAM. You will also probably need 64bit versions of the compiler's executable because it might consume more than 4 GB of RAM. Run "make run-gcc-quick" or "make run-clang-quick" to only perform a limited set of tests that compile much faster.

\section Documentation
- \subpage tutorial "The In-Depth Tutorial" covers all concepts of µSer
- \subpage Examples "List of all examples"
- \ref Attr "List of attributes"
- Documentation for the \ref uSer::serialize "serialize" and \ref uSer::deserialize "deserialize" functions


\page tutorial Tutorial
This page explains all concepts of using µSer.
\tableofcontents
\section ConDataStr Data Structures
µSer's main task is to deal with different kind of data structures designed by the library's user. The object passed to \ref uSer::serialize serialize or \ref uSer::deserialize deserialize is the root of a tree-like data structure. The leaves are always integral or enum types, and the inner nodes various kinds of containers - similar to e.g. JSON, but the structure is fixed by the application's source code.

All standard integral types are supported by µSer: bool, char, char16_t, char32_t, wchar_t, short, int, long, long long and signed/unsigned variants. All extension types for which std::numeric_limits, std::is_signed_v and std::is_unsigned_v are specialized and for which std::is_integral_v<T> is true work as well. Enumeration types are casted to/from their underlying integral type for (de)serialization.

Integers may be stored inside container types, which in turn can be stored in more containers. µSer supports different kinds of containers: C-Arrays, std::array and all other homogeneous containers supporting iterators as well as structs that have non-static member variables, std::tuple and std::pair, which can be considered heterogeneous containers. µSer never resizes containers - before deserialization, the user has to allocate elements that µSer will write to. Structs have to be \ref ConStructAnnot "annotated" to make their members known to µSer; all other containers can be directly used. The contained elements are (de)serialized consecutively.

We have already seen how to serialize an integer:
\include 01_basic_write.cpp

\remark µSer only supports unsigned integers in the raw data, as bit operations on signed integers would be largely platform-dependent. Unfortunately, C++'s I/O functions usually work on "char", which might be signed. Converting unsigned to signed data is platform-dependent (see paragraph [conv.integral] in the C++ Standard), so converting each element of the raw data individually is not a solution. Using reinterpret_cast to convert the pointer to unsigned data into a pointer to signed data is implementation-defined as well. Since it keeps the bit patterns intact, it can be expected to transfer the bits of the unsigned data directly to/from the file and works on all major platforms. This method will be used in all further examples; the user code has to make sure that the (de)serialized unsigned integers are correctly read/written on the target platform.

Serializing a container essentially works the same way:

\include 03_ser_array.cpp

This example outputs "TEST" too, but this time we packed two bytes into one 16-Bit-Integer. 

Tuples allow us to combine values of different types:
\include 04_ser_tuple.cpp
The output still stays the same.

Until now, µSer was able to check at compile time whether the data fits into the raw array, since the sizes of both are fixed. This is not possible when a container of dynamic size, such as std::vector, is used. In that case the container might be too large to fit into the raw data array. µSer then automatically implements a size check to prevent buffer overflow. To react to an error condition appropriately, we need to implement some error handling. The easiest way to do that is to defined the macro \ref USER_EXCEPTIONS _before_ including the \ref uSer.hh "uSer.hh file" to enable exception support, and catch the exceptions:

\include 05_ser_vector.cpp

Adding another element to the vector will provoke the error. See \ref ConError for more information. As serializing structs needs some extra work, it is documented in \ref ConStructAnnot.

\section ConRaw Raw Data
Before we dive further into the API documentation, we need to think about how data is mapped from the raw binary stream to the C++ data structures. This is merely conceptual; µSer does calculations on whole integers and not individual bits.

As stated previously, µSer (de)serializes data to/from a stream of unsigned integers of equal type. We will call these integers "serialization word" or short "SWord". Typically, std::uint8_t or unsigned char will be used here. Depending on the architecture, using a larger type may improve performance, particularly if the size of a SWord corresponds to the register size of the processor. For example, on a 32-Bit-Architecture std::uint32_t may yield best results. However, since the size of the elements of \ref ConDyn "dynamic data structures" must be a multiple of the SWord size, the size of the SWord may not be chosen arbitrarily depending on the data structures.

The raw binary in/out data is seen as a stream of bits which are made up of the sequence of SWords. The least significant bit of the SWords always comes first, and the most significant one comes last. If you serialize some data into a std::uint8_t stream, then combine pairs of those into std::uint16_t integers in a little endian fashion and deserialize the resulting stream, you should end up with the same data. If some communication channel combines 8-Bit-Integers into 16-Bit-Integers in a big endian fashion, the above rule would be violated - bits 8-16 of the first integer would come first, and then bits 0-7. To deserialize such a data stream, you have to swap the bytes manually or by calling \ref uSer::deserialize a second time (see below for explanation of the API usage):

\include 06_be_raw.cpp

The bits in one serialization word can be configured by the \ref uSer::RawInfo attribute. This can be used to e.g. serialize data into 7-bit-Words or other word sizes for which the platform offers no integer type. The serialization word type must be large enough to store those bits.

Of the tree formed by the C++ data structure, only the leaves, i.e. the integers (or enums converted to integers) are mapped to the bit stream. No meta information about the types or container sizes is written or read such that the application can implement most given formats. In essence, all integers and enums in the data structure are mapped to a "flat" sequence of integers of different size whose bits are then mapped to the raw data stream. An integer may be split into bytes whose order can be configured as the \ref ConBO "byte order" (usually \ref uSer::ByteOrder::LE "little" or \ref uSer::ByteOrder::BE "big" endian). Of each byte the least significant bit is serialized first, and the most significant one last. This means that the bytes are always stored "forwards" in the raw data stream and never reversed. If individual integers or all SWords need to be reversed, this has to be done manually.

\section ConAttr Attributes

µSer's behaviour can be controlled by attributes. These are a set of types, some of them templates, that are passed via type parameters to µSer. There are never instances made of those types, and they provide no member functions for application code. Attributes are valid for one object. If they are used on a container type, they are applied to the contained elements. An exception are structs: Attributes applied to a struct are only used on the members if they are marked as inheritable. The easiest way to specify attributes is by passing them as template parameters to the \ref uSer::serialize "serialize" and \ref uSer::deserialize "serialize" functions:

\include 07_attr_funarg.cpp

The \ref uSer::ByteOrder::BE attribute configures the \ref ConBO "byte order" to big endian, which reverses the string written to standard output. The \ref uSer::Width attribute sets the size of the integer explicitly, see \ref ConWidth. Entire structs and their members can be annotated with attributes as well, see \ref ConStructAnnot "below" for details.

The following attributes are available:

<table>
<caption>µSer Attributes</caption>
<tr><th>Category</th><th>Name</th><th>Default</th><th>Inheritable</th><th>Description</th></tr>
<tr>
	<td rowspan="3">Byte Order</td>
	<td>\ref uSer::ByteOrder::LE</td><td align="center">✔</td><td align="center">✔</td><td>Serializes integers in little endian byte order, i.e. the least significant byte first.</td> 
</tr>
<tr>
	<td>\ref uSer::ByteOrder::BE</td><td></td><td align="center">✔</td><td>Serializes integers in big endian byte order, i.e. the most significant byte first.</td>
</tr>
<tr> 
	<td>\ref uSer::ByteOrder::PDP</td><td></td><td align="center">✔</td><td>Serializes integers in PDP-endian byte order, i.e. serialize 32bit-Integers as two 16bit-integers, the most significant one first, and each internally as little endian.</td> 
</tr>
<tr>
	<td rowspan="3">Sign Format</td>
	<td>\ref uSer::SignFormat::TwosComplement</td><td align="center">✔</td><td align="center">✔</td><td>Stores signed integers in 2's complement, the standard for most architectures. The top half of the unsigned integer range is mapped on the negative numbers until -1, keeping the order.</td>
</tr>
<tr>
	<td>\ref uSer::SignFormat::SignedMagnitude</td><td></td><td align="center">✔</td><td>Stores signed integers in Signed-Magnitude format, i.e. an absolute value and a sign bit that defines whether the value is positive or negative.</td>
</tr>
<tr>
	<td>\ref uSer::SignFormat::OnesComplement</td><td></td><td align="center">✔</td><td>Stores signed integers in 1's complement, similar to Signed-Magnitude but the absolute value is bitwise negated for negative values.</td>
</tr>
<tr>
	<td rowspan="2">Padding</td>
	<td>\ref uSer::Padding::None</td><td align="center">✔</td><td align="center">✘</td><td>No additional dummy bits after integers.</td>
</tr>
<tr>
	<td>\ref uSer::Padding::Fixed</td><td></td><td align="center">✘</td><td>Stores a given fixed amount of bits after an integer, e.g. to accommodate alignment requirements.</td>
</tr>
<tr>
	<td rowspan="2">Dynamic Data</td>
	<td>\ref uSer::Dyn::Size</td><td></td><td align="center">✘</td><td>Define the size of a container depending on runtime information</td>
</tr>
<tr>
	<td>\ref uSer::Dyn::Optional</td><td></td><td align="center">✘</td><td>Define optional data objects depending on runtime information</td>
</tr>
<tr>
	<td rowspan="4">Hooks</td>
	<td>\ref uSer::Hook::SerPre</td><td></td><td align="center">✘</td><td>Invoke a user-provided callback function _before_ serializing an object</td>
</tr>
<tr>
	<td>\ref uSer::Hook::SerPost</td><td></td><td align="center">✘</td><td>Invoke a user-provided callback function _after_ serializing an object</td>
</tr>
<tr>
	<td>\ref uSer::Hook::DeSerPre</td><td></td><td align="center">✘</td><td>Invoke a user-provided callback function _before_ deserializing an object</td>
</tr>
<tr>
	<td>\ref uSer::Hook::DeSerPost</td><td></td><td align="center">✘</td><td>Invoke a user-provided callback function _after_ deserializing an object</td>
</tr>
<tr>
	<td rowspan="1">Integer Width</td>
	<td>\ref uSer::Width</td><td></td><td align="center">✔</td><td>Manually define the size of an integer in bits</td>
</tr>
<tr>
	<td rowspan="1">SWord information</td>
	<td>\ref uSer::RawInfo</td><td></td><td align="center">✘</td><td>Explicitly define the serialization word and optionally its size; useful if an iterator is used for which std::iterator_traits<>::value_type is not defined, e.g. std::back_insert_iterator. Only valid in the argument list of \ref uSer::serialize "serialize" and \ref uSer::deserialize "deserialize".</td>
</tr>
</table>

The attributes belonging to one category are mutually exclusive; at maximum one of them may be defined on an object. If a sub-object has an attribute defined that conflicts with an inheritable attribute of a surrounding object, the attribute of the sub-object takes precedence. For example, if a struct is annotated with \ref uSer::ByteOrder::LE but a member is annotated with \ref uSer::ByteOrder::BE, the latter is effective for that member and its sub-objects, if any. The complete reference to attributes is found \ref Attr "here".

\section ConAPI The serialize and deserialize functions

The main entry points of µSer's API are the functions \ref uSer::serialize "serialize" and \ref uSer::deserialize "deserialize" which have several overloads to accommodate different use cases. The two functions have almost identical signatures that only differ in const-ness of the first two parameters. Because simply listing all overloads wouldn't be much help, we'll look at the functions on a more abstract level: 
\code{.unparsed}
	void|uSer_ErrorCode serialize   (raw, const obj [, size] , std::size_t* sizeUsed = nullptr)
	void|uSer_ErrorCode deserialize (const raw, obj [, size] , std::size_t* sizeUsed = nullptr)
\endcode
The meaning of the parameters is:
- The "raw" parameter refers to the raw binary data stream. It can be a reference to a C-Array, std::array, a container providing a ".size()" member function, or an \ref ConIter "iterator" containing unsigned integers, the \ref ConRaw "serialization words". For deserialize, the array/container or the target for the iterator may be const. µSer never allocates elements in output containers; the user has to make sure enough elements are available for writing, or use an iterator that appends as needed, such as std::back_insert_iterator.
- The "obj" parameter is a reference to the \ref ConDataStr "C++ data structure" to be (de)serialized, i.e. an integer, enum, struct, std::pair, std::tuple or container supporting iterators. For serialize, the object may be const.
- The "size" parameter specifies the size of the raw data stream, i.e. how many serialization words are available. It may be omitted if "raw" refers to a C-Array (an actual array, not a pointer to the first element), a std::array, or any container providing a ".size()" member function, in which case its size is used. The size may be specified in different ways:
	- An integer of type std::size_t specifying the number of available serialization words.
	- An instance of \ref uSer::DynSize containing a std::size_t; this is equivalent of specifying a naked std::size_t.
	- An instance of \ref uSer::FixedSize, conveniently obtained by writing "uSer::fixedSize<N>", signifying a compile-time known fixed buffer size.
	- An instance of \ref uSer::InfSize, conveniently obtained by writing "uSer::infSize", signifying an infinite buffer size, e.g. a socket of file handle.
	
	When using \ref uSer::FixedSize for the size parameter or when using an array type for "raw", µSer can check the buffer size at compile time. If it is to small (excluding \ref ConDyn "dynamic data structures"), µSer will emit an error via static_assert.
- The sizeUsed parameter is a pointer to std::size_t. If it is not a null pointer (the default), µSer will write the number of actually read/written serialization words to its target. This is useful when dynamic data structures are used.

We have already seen how to serialize to arrays:
\include 01_basic_write.cpp

Serializing into a vector works similarly:
\include 08_ser_raw_vector.cpp

Note that the vector was initialized with 4 elements which are overwritten by serialize. If a number smaller than 4 were passed to std::vector's constructor, an exception would be thrown.

When "uSer::fixedSize<N>" is passed as the "size" parameter and the data structure contains no dynamic data, no runtime buffer range checks are performed, and no error handling is necessary:
\include 09_ser_raw_vector_fsize.cpp
\note You have to ensure that the parameter to uSer::fixedSize matches the actually available data. If the buffer is actually smaller, undetected buffer overflows and security holes might occur!

We can pass a std::size_t or a \ref uSer::DynSize as the size parameter to only (de)serialize a part of a container with a runtime-known size:

\include 10_deser_raw_vector_dynsize.cpp

Since the size is not known in advance, error handling is required.
When there is no explicit limit to the raw buffer size, we can pass "uSer::infSize" as the size argument. This can e.g. be used in combination with std::back_inserter to automatically grow the container as needed. Since µSer doesn't check the buffer size in this case, error handling is not needed unless other things require error handling (dynamic data structures). Unfortunately, the C++ Standard Library offers no way to determine the serialization word type from a given std::back_inserter_iterator type. Therefore, we have to explicitly pass the serialization word to the seralize function via the \ref uSer::RawInfo attribute:

\include 11_ser_raw_vector_infsize.cpp

This is also the first example where an \ref ConIter "iterator" instead of a reference to a container is passed to deserialize.
\subsection ConError Error Handling

The return type of serialize and deserialize depends on the parameters and is used to signal error conditions. If \ref USER_EXCEPTIONS is defined _before_ including the \ref uSer.hh header, exceptions are used to signal errors, and the return type will always be void. If it was not defined, and an error is possible (e.g. when using dynamic data structures or dynamic buffer sizes), the return type will be \ref uSer_ErrorCode to signal success (\ref uSer_EOK) or failure. If no errors can happen, the return type will be void. The \ref uSer_ErrorCode enum is defined as "[[nodiscard]]" - if you ignore the returned value, the compiler will emit a warning. If you use the return value even if no errors are possible, you will get a compiler error. This prevents both forgetting to include error handling and superfluous error handling code.

We have already seen how to use exceptions:
\include 05_ser_vector.cpp

When using return codes, you can user \ref uSer_getErrorMessage to retrieve a string describing the error:

\include 12_errorcode.cpp

If you were to ignore the returned value in this example, the compiler will issue a warning.

\remark Exceptions are generally the better way to handle errors, as they allow to free resources automatically ("RAII") and pass errors through multiple levels of independently developed software components. They can also actually have a better performance than error codes, because program execution will directly jump to the catch-handlers in case of an error, while return codes have to be checked repeatedly to decide whether execution should continue or cancel. However, on small embedded systems, the runtime library for handling exceptions might be simply too large for the limited program memory, which is why you can disable exception handling in µSer. If you are programming for a PC or an embedded system running a "full" operating system like Linux/Android, using exceptions is probably the best way.

\section ConStructAnnot Defining and Annotating structs

In C++, struct and class are actually almost the same thing: Both keywords declare classes (technically, there are no structs in C++) with the only difference that members and base classes of classes declared by "struct" are public by default, and private for those declared by "class". However, it is customary to use "struct" for simple classes that only contain "flat" data without encapsulation mechanisms such as getter/setter functions, similar to how they are used in C. Therefore, we will call these classes "structs". Typically, most serializable data in applications is declared in such structs, e.g. to contain data for network packets or file headers. Therefore, structs play a central role in µSer.

Unlike with containers, tuples and arrays, there is no way to automatically get a list of the members of a struct. This makes it necessary to explicitly make the list of members known to µSer.

\subsection ConStruct1 Defining and annotating struct members separately
The easiest way to do this is as follows:

\include 13_struct.cpp

First, the struct is defined as usual. Then a call to the \ref USER_STRUCT macro is placed at the beginning. It requires one parameter, which is the name of the struct. There may be further parameters that define attributes valid for the whole struct. In this example, we don't provide any attributes. A strict compiler might emit a warning or error message because empty variadic macro parameters are not permitted by the C++ standard; in that case, we can pass \ref uSer::AttrNone as a dummy meaning "no attributes".

\subsection ConStruct2 Annotating a whole struct
Declaring attributes for a whole struct looks this way:
\include 14_struct_attr.cpp
Now, all members of the struct are serialized as big endian.

\subsection ConStruct3 Annotating a single struct member
A single member can be annotated by using \ref USER_MEM_ANNOT :
\include 15_struct_memattr.cpp

\subsection ConStruct4 Defining and annotating a single member in one step
The definition and annotation of individual members can be combined by using \ref USER_MEM :
\include 16_structmem_compact.cpp
This method does not support arrays since their syntax requires parts of the type to be placed _after_ the member name.

\subsection ConStruct5 Defining and annotating all struct members in one step
The most compact method to declare structs is provided by \ref USER_DEF_MEM :
\include 17_struct_compact.cpp
This variant defines annotations for all members in one step. Again, we may have to use \ref uSer::AttrNone to prevent empty variadic argument list. This variant does not support arrays either, but avoids typing every variable name twice.

\subsection ConStruct6 Non-Intrusive struct annotation
The previously presented methods require the modification of the actual struct definition. They all add some some member types and static member functions to the struct whose names start with "uSer_". If the struct definition cannot be changed (e.g. because it belongs to an external library) or adding those members is not desirable (even though they have no influence on the struct's runtime behaviour), structs can be defined in a non-intrusive fashion. The macros used to achieve this have to be used in the _global_ namespace. If the struct is defined in some namespace, its name has to be fully qualified.

Attributes valid for the whole struct can optionally be defined by \ref USER_EXT_ANNOT . If no attributes are needed, this macro can be omitted. Individual members can be annotated by \ref USER_EXT_MEM_ANNOT . The only required macro is \ref USER_EXT_ENUM_MEM, which defines the list of members.
\include 18_struct_ext.cpp

This can also be done in a more compact way by using \ref USER_EXT_DEF_MEM which defines and annotates all members at once. Attributes for the whole macro can again optionally be defined by \ref USER_EXT_ANNOT .
\include 19_struct_ext_compact.cpp

It is actually possible to annotate any serializable type with \ref USER_EXT_ANNOT . Annotating general types such as "int" or "std::vector<short>" is however discouraged, since the annotation will be valid for any serialization process and might affect the behaviour of other parts of the program.

\section ConBO Byte Order

An important aspect for serializing integers is the byte order. Integers that have more bits than a byte need to be split into multiple bytes for storage. Most platforms enforce a particular byte order by storing integers in memory in a certain way. If the data is copied from memory and written to a file directly, that byte order is applied to the stored data as well. If that file is copied to a computer with a different byte order, loaded into memory directly, and processed by arithmetic operations, unexpected results may occur. The most popular byte order is \ref uSer::ByteOrder::LE "little endian", where the least significant byte is stored first and the most significant one last. This byte order is e.g. used by x86 and most ARM platforms. The reverse case is \ref uSer::ByteOrder::BE "big endian", where the most significant byte is stored first. This is used by PowerPC and in various internet protocols. \ref uSer::ByteOrder::PDP "PDP endian" is a hybrid variant, where 32-bit-Integers are split into two 16-bit-Integers, where the most significant one is stored first. The two 16-Bit-Integers are internally stored as little endian. This order stems from the PDP architecture.

For example, the number 305419896, or hexadecimal 0x12345678, is stored in the following orders (all numbers hexadecimal):
<table><caption>Byte Order examples</caption>
	<tr><th>Byte Order</th><th>Address 0</th><th>1</th><th>2</th><th>3</th></tr>
	<tr><td>Little Endian</td><td align="center">78</td><td>56</td><td>34</td><td>12</td></tr>
	<tr><td>Big Endian</td><td align="center">12</td><td>34</td><td>56</td><td>78</td></tr>
	<tr><td>PDP Endian</td><td align="center">34</td><td>12</td><td>78</td><td>56</td></tr>
</table>

µSer offers functionality to convert the data from the local platform's byte order into a specific defined order and back. The application code defines a fixed order desired for raw binary data, i.e. the network protocol or file format. µSer automatically converts the C++ data in the local order to/from that order. Since this is done via bit-operations, the order of the local platform need not be explicitly known and could actually be an entirely different one.

We have already seen how to use attributes to request a certain byte order:
\include 14_struct_attr.cpp
If no byte order is defined, µSer assumes little endian. If we serialize this struct on one platform and then deserialize it on another, µSer guarantees that the data in the integers is correct, regardless of the byte orders of the two platforms.

When the \ref uSer::Width "Width" attribute is used to define an integer size that is not a multiple of a byte, the "incomplete" byte is assumed to contain the remaining most significant bits. In big endian order, this incomplete byte then comes first. Consider this example:
\include 20_byte_order.cpp
The output is:

\code{.unparsed}
0x65, 0x7, 
0x2f, 0x3, 
\endcode

\section ConSF Sign Formats
Like with byte orders, different platforms have different ways to store negative values. µSer supports three of those: \ref uSer::SignFormat::TwosComplement "Two's complement", \ref uSer::SignFormat::OnesComplement "One's complement" and \ref uSer::SignFormat::SignedMagnitude "Signed-Magnitude".

The format two's complement takes the top half of the corresponding unsigned integer's range, and translates it into the negative numbers. For example, for a 16-Bit-Integer, the numbers 0-32767 stay as they are. The binary representation of the unsigned number 32768 means -32768 in two's complement, 32769 means -32767, and 65535 means -1. This is the most popular format used on most platforms, and the other two are rare.

The signed-magnitude format stores an absolute value and a sign bit that defines whether the value is positive or negative. This format corresponds to the usual decimal notation, where the a sign bit of "1" means "-" and "0" means "+". Inverting the sign bit means negating the value. This format has two representations for zero, i.e. +0 and -0.

The one's complement format is similar to signed-magnitude but the absolute value is bitwise negated for negative values. This format has two representations for zero as well. Inverting all bits means negating the number.

Just like the byte order, the application defines the desired sign format, and µSer converts the local format from/to the desired one. This works independently of the host's format. The ranges of the different formats are not equal: For example, 16-bit two's complement numbers have the range -32768 - 32767. 16-bit-Integers in the other two formats have the range -32767 - 32767. µSer uses static assertions to make sure the integer in the raw data fits into the local format. For example, if the local platform uses signed-magnitude, and you try to deserialize a 16-bit-integer in two's complement into a int16_t, you will get a compiler error, since -32768 can not be represented on the host platform.

This example serializes a signed integer in all three sign formats:
\include 21_sign_format.cpp
The output is:
\code{.unparsed}
0xdd, 0xfe, 
0xdc, 0xfe, 
0x23, 0x81,
\endcode

\section ConWidth Integer Sizes

The different platforms support different integer sizes, but a protocol might require integers of a size for which no type exists. µSer allows to set a specific fixed size for an integer by using the \ref uSer::Width "Width" attribute. The next integer will be serialized right after the previous one; integers need not start at a byte order. Essentially, this simulates bitfields in the binary data stream, without the need for actual C++ bitfields which behave in a non-portable way.

A simple example is to convert RGB565 color values into a single 16bit-Integer:
\include 22_integer_width.cpp

\section ConPadding Padding

Some formats require unused bits between the individual values. These are ignored during reading, and typically written to zero. These unused bits are called padding bits. While it is possible to define "dummy" integers (possibly using \ref uSer::Width to achieve a specific amount of bits) that receive the padding bits, this wastes memory. Therefore, µSer provides the \ref uSer::Padding::Fixed "Padding::Fixed" attribute which can only be applied to integers, and specifies a fixed amount of padding bits that come after that integer.

Assuming we want to skip the "green" component from the previous color example, we can add 6 padding bits after the "red" one:
\include 23_integer_padding.cpp
The remaining two values retain their position in the raw data, and the "gap" is filled with zero bits.

\section ConIter Raw Iterators

µSer accesses the raw data stream via iterators. In C++, an iterator is a small class that "refers" to an element in a container or an input/output stream. Unlike a simple index, an iterator instance knows which container it refers to. A pointer is the simplest kind of iterator, to be used with C-Arrays. The first parameter to the \ref uSer::serialize "serialize" and \ref uSer::deserialize "deserialize" functions is an iterator to the first raw data element (or a container/C-Array, of which the iterator to the first element will be queried using std::begin). µSer supports all standard library iterators that refer to unsigned integers, but it is possible to implement your own iterators to e.g. directly read/write the raw serialization words from/to some communication interface, network or file.

We can use std::istream_iterator and std::ostream_iterator to read/write raw data directly from/to files:
\include 24_stream_iterators.cpp
Note that \ref uSer::RawInfo "RawInfo" is needed for the output operator, since µSer can't automatically determine the serialization word type from std::ostream_iterator. The iterators are instantiated with "std::uint8_t" since µSer needs unsigned integers in the raw stream. The file size is queried ahead of deserialization, as the input iterator doesn't signal end-of-file to µSer. 

µSer's requirements for iterators are actually weaker than the standard library's, making it easy to write your own ones. With "T" being the iterator type, "iter" an instance of "T", and "x" an instance of the serialization word, the basic requirements imposed by µSer are:
- T needs to be move-constructible (i.e. have a constructor of the form "T::T (T&&)")
- T needs to be move-assignable (i.e. have an assignment operator of the form "T& operator = (T&&)")
- T does not have to (but may) be default-constructible, copy-constructible or copy-assignable.
- T needs the following member type definitions:
	- "T::iterator_category" must be one of the standard iterator tag types; see the seven cases below for details.
	- "T::value_type" may be an alias for the serialization word type. If it is void, you must use \ref uSer::RawInfo "RawInfo" to make that type known to µSer.
	- "T::difference_type" must be an integral type large enough to store the difference between two positions
	- "T::pointer" and "T::reference" must be a pointer/reference type to the serialization word or "void"; not used by µSer but required for std::iterator_traits<T> to work
- For \ref uSer::deserialize "deserialize", "x = (*iter)" should return the current serialization word from the raw stream
- For \ref uSer::serialize "serialize", "(*iter) = x;" should write the next serialization word to the raw stream
- "++iter" should advance the iterator to the next element

If the raw binary stream contains bytes that consist entirely of padding bits, µSer is able to skip over those efficiently. There are seven cases for different kinds of iterators depending on whether padding bytes exist, each with their own set of guarantees and requirements:

\subsection ConIter1 Input Iterators with no padding bytes
In this case, "T::iterator_category" is not used. µSer will call "*iter" and "++iter" in strictly alternating order, e.g.:

\code{.unparsed}
x = *iter;
++iter;
x = *iter;
++iter;
x = *iter;
++iter;
\endcode

\subsection ConIter2 Output Iterators with no padding bytes
In this case, "T::iterator_category" is not used. µSer will call "(*iter)=x" and "++iter" in strictly alternating order, e.g.:

\code{.unparsed}
(*iter) = x;
++iter;
(*iter) = x;
++iter;
(*iter) = x;
++iter;
\endcode

This makes it easy to implement both input/output iterators, since the actual read/write can be done via the "operator *" while not doing anything in "operator ++". An example for both is:

\include 25_user_iterator1.cpp

\subsection ConIter3 Input Iterators with padding bytes and without operator +=
In this case, "T::iterator_category" is not used. Two calls to "*iter" will never occur in direct succession, but multiple calls to "++iter" might directly follow each other to skip over padding bytes:

\code{.unparsed}
x = *iter;
++iter;
x = *iter;
++iter;
++iter;
x = *iter;
++iter;
\endcode

\subsection ConIter4 Input Iterators with padding bytes and with operator +=
In this case, "T::iterator_category" is not used. With "n" being an instance of std::iterator_traits<T>::difference_type, "iter += n" should skip over n bytes. Two calls to "*iter" will never occur in direct succession, "++iter" will always follow "*iter", "iter += n" can optionally follow a "++iter", and "*iter" will follow either "++iter" or "iter += n", e.g.: 

\code{.unparsed}
x = *iter;
++iter;
x = *iter;
++iter;
iter += 2;
x = *iter;
++iter;
\endcode

\subsection ConIter5 Output Iterator with padding bytes and T::iterator_category = std::output_iterator_tag
In this case, µSer will call "(*iter)=x" and "++iter" in strictly alternating order and padding bytes will be written as zero, e.g.:

\code{.unparsed}
*iter = x;
++iter;
*iter = 0;
++iter;
*iter = x;
++iter;
\endcode

\subsection ConIter6 Output Iterator with padding bytes, without operator += and T::iterator_category = std::forward_iterator_tag
This section applies for iterators where std::iterator_traits<T>::iterator_category is exactly or convertible to std::forward_iterator_tag. In this case, two calls to "*iter" will never occur in direct succession, but multiple calls to "++iter" might directly follow each other to skip over padding bytes:

\code{.unparsed}
*iter = x;
++iter;
*iter = x;
++iter;
++iter;
*iter = x;
++iter;
\endcode

\subsection ConIter7 Output Iterator with padding bytes, with operator += and T::iterator_category = std::forward_iterator_tag
This section applies for iterators where std::iterator_traits<T>::iterator_category is exactly or convertible to std::forward_iterator_tag. With "n" being an instance of std::iterator_traits<T>::difference_type, "iter += n" should skip over n bytes. Two calls to "(*iter)=x" will never occur in direct succession, "++iter" will always follow "(*iter)=x", "iter += n" can optionally follow a "++iter", and "(*iter)=x" will follow either "++iter" or "iter += n", e.g.: 

\code{.unparsed}
*iter = x;
++iter;
*iter = x;
++iter;
iter += 2;
*iter = x;
++iter;
\endcode

\section ConDyn Dynamic data structures

Data structures are considered dynamic if their size is not known at compile time, but is determined at runtime based on data available only then. For deserialization, the size of a dynamic data structure might depend on other data objects that have just been deserialized. The serialized size of any dynamic data structure must be equal to or a multiple of the size of a serialization word. This restriction ensures that data that comes after the dynamic data always starts at the same bit in one serialization word, which greatly improves performance. For example, if you want to serialize a std::vector<std::uint16_t>, the serialization word can only be std::uint8_t or std::uint16_t, but not std::uint32_t. Since dynamic data structures always have the potential to overflow the raw buffer, error handling is required unless \ref uSer::InfSize "InfSize" is specified as the buffer size.

µSer supports three different kinds of dynamic data:

\subsection ConDyn1 Containers with dynamic size
When serializing a container that is not std::array or a C-Array, µSer will assume it to be dynamic, and query its size by using its ".size()" member function. µSer will serialize or deserialize exactly as many elements. This means that the prior to deserialization, the container needs to be set to its desired size, as µSer will never resize containers.

We have already seen how to serialize std::vector: 
\include 05_ser_vector.cpp

Deserialization then works like this:
\include 26_deser_vector.cpp
Note how the vector is initialized to have 2 elements. Passing a greater integer will cause an exception as the buffer would overflow.

\subsection ConDyn2 Dynamic size of struct members via Dyn::Size

By annotating a container member of a \ref ConStructAnnot "struct" with the \ref uSer::Dyn::Size "Dyn::Size" attribute, you can make its size depend on various kinds of runtime data. The sole argument to Dyn::Size is a reference which tells µSer what size the container has. The reference can be:
-# A non-static member function of the surrounding struct
-# A free function (=function pointer)
-# A static member function of any class (=function pointer)
-# A pointer to a global object that has operator () overloaded
-# A member variable pointer to a member of the surrounding struct, which must be convertible to std::size_t.
In the first four cases, the return type must be convertible to std::size_t. For cases 2-4, a reference to the struct
is passed as an argument.

This example shows four variants:

\include 27_dynsize.cpp

\subsection ConDyn3 Optional struct members via Dyn::Optional

By annotating a container member of a \ref ConStructAnnot "struct" with the \ref uSer::Dyn::Optional "Dyn::Optional" attribute, you can make its presence depend on various kinds of runtime data. The sole argument to Dyn::Optional is a reference which tells µSer whether the object exists. For the reference, the same rules as for Size apply, but the return type has to be (convertible) to bool.

This example shows four variants:

\include 28_optional.cpp

The variables N1-N4 are declared as std::uint8_t to avoid odd data in the example.

\section ConHooks Hooks

Sometimes it is necessary to do some application-specific calculations and checks during (de)serialization. µSer accommodates this by allowing the application to specify hook functions which will be called before or after an object is (de)serialized. This can be achieved by the four attributes \ref uSer::Hook::SerPre "Hook::SerPre", \ref uSer::Hook::SerPost "Hook::SerPost", \ref uSer::Hook::DeSerPre "Hook::DeSerPre", \ref uSer::Hook::DeSerPost "Hook::DeSerPost". These attributes take a reference to a function which will be called before/after the annotated object is (de)serialized.
The argument may reference:
-# A non-static constant (for serialization) or non-constant (for deserialization) member function of the surrounding struct
-# A free function (=function pointer)
-# A static member function of any class (=function pointer)
-# A pointer to a global object that has operator () overloaded
-# If the annotated object is a struct, and the annotation is *not* given as a member annotation of a surrounding struct,
		the reference may be a non-static constant (for serialization) or non-constant (for deserialization) member function of that struct.

The return type can be:
-# void, in which case the function may indicate errors via exceptions, but not fail otherwise
-# \ref uSer_ErrorCode, in which case the function may indicate errors by the returned value or via exceptions (if enabled). If the function
		indicates an error via a return value and exceptions are enabled, µSer will throw an exception that forwards the error code.

A constant (for serialization) or non-constant (for deserialization) reference to the annotated object is passed as the first argument. For cases 2-4, a constant (for serialization) or non-constant (for deserialization) reference to the struct is passed as a second argument, if the attribute was applied to a struct member.

An example for the different reference types and hook types is:
\include 29_hook.cpp
Note that during the pre-deserialization hook, the object might not contain any meaningful data.

\section ConBufSize Calculating buffer sizes

µSer allows you to determine the size of the raw buffer based on the C++ data structure during compilation. This can be useful to allocate buffers of appropriate size.
- \ref uSer::rawStaticBits "rawStaticBits<RawIter, T, Attr...>" returns the number of fixed bits belonging to the non-dynamic part of T in the raw data stream referred to by the raw iterator type RawIter when serializing with the attributes Attr.
- \ref uSer::RawMaxDynamic "RawMaxDynamic<RawIter, T, Attr...>" is an alias to either \ref uSer::MaxSize "MaxSize", \ref uSer::Unlimited "Unlimited" or \ref uSer::NoDyn "NoDyn" indicating the upper limit of dynamic serialization words in its member value, that there is no upper limit, or that there is no dynamic data, respectively.
- \ref uSer::minBufSize "minBufSize<RawIter, T, Attr...>" is an integer denoting the minimum number of serialization words needed to (de)serialize the type T.
- \ref uSer::minBufSize "maxBufSize<RawIter, T, Attr...>" is an integer denoting the maximum number of serialization words needed to (de)serialize the type T. Using this alias may cause a compiler error if the dynamic size is unlimited. Note that the maximum size might actually never occur depending on the data structure.

\section Minimize Using µSer on resource-constrained systems
µSer is designed to work on resource-constrained systems, such as small embedded systems. To this end, µSer never does any dynamic memory allocation. There are also some macros that can be defined _before_ including the uSer.hh header (or via the compiler's command line) to configure µSer:
- \ref USER_EXCEPTIONS Do _not_ define this macro in order to disable exception support, which typically consumes a large amount of program memory
- \ref USER_NO_PRINT Define this macro to disable support for printing serializable data via the \ref uSer::print function and prevent inclusion of the &lt;ostream&gt; header.
- \ref USER_NO_SMARTPTR Define this macro to disable support for smart pointers and prevent inclusion of the &lt;memory&gt; header.

When using GCC, compile with "-fdata-sections -ffunction-sections -flto", and link with "-Wl,--gc-sections -flto". These options can reduce the amount of program memory needed. µSer also relies on optimization to be turned on (e.g. -O2 for GCC and Clang) to generate efficient code. µSer employs deeply nested call stacks to statically build algorithms specifically adapted to the user-defined data structures. With optimization enabled, the compiler collapses those into short and efficient algorithms. If some code does not work with optimizations enabled, this is most probably the result of relying on some undefined behaviour, i.e. programming errors. Using µSer instead of e.g. pointer casts to serialize data already avoids some of those problems (specifically padding, data alignment, aliasing rules).

\section CCompat C-Compatibility
µSer can be used in C-based projects if a C++17-compatible compiler is available. Fist, define the desired data structures in a common header for both C and C++ (here: packet.h):
\include C/packet.h
Define the structs by using the µSer-provided macros \ref ConStructAnnot "as explained before". Also declare serialize/deserialize functions as needed, i.e. for the structs we want to explicitly serialize from C code. In this example, we only want to (de)serialize PacketB from C, and have the contained PacketA instances (de)serialized automatically. Therefore, we don't need serialization functions for PacketA. Adjust the signature as needed (with or without error codes in the return type, the desired raw buffer type, with or without a buffer size parameter). In order to call a C++ function from C, it has to be annotated with 'extern "C"', but only when the C++ compiler sees it. The \ref USER_EXTERN_C macro can be used for this, which evaluates to 'extern "C"' when compiling as C++, and to nothing when compiling as C.

Then, create a C++ source file for implementing the serialization functions:
\include C/serialize.cpp
We have to include the "packet.h" file, and just call \ref uSer::serialize "serialize"/\ref uSer::deserialize "deserialize" in the function body. Remember to switch on the compiler's C++17 support (e.g. -std=c++17 for GCC and Clang). We can then use these functions from C code, e.g.:
\include C/main.c

\page Examples Examples
µSer ships with a set of examples in the "examples" subdirectory. Run "make" in that directory to compile them all. The "examples/C" directory contains an \ref ExC "example" for using µSer with a C project consisting of multiple files.

\tableofcontents
\section Ex01 Serializing an integer into an array
\include 01_basic_write.cpp
\section Ex02 Deserializing an integer from an array
\include 02_basic_read.cpp
\section Ex03 Serializing an array of integers into an array
\include 03_ser_array.cpp
\section Ex04 Serializing a tuple of integers into an array
\include 04_ser_tuple.cpp
\section Ex05 Serializing a vector of integers into an array
\include 05_ser_vector.cpp
\section Ex06 Pre-Processing raw data with swapped bytes
\include 06_be_raw.cpp
\section Ex07 Specifying attributes via function arguments
\include 07_attr_funarg.cpp
\section Ex08 Serializing an integer into a std::vector
\include 08_ser_raw_vector.cpp
\section Ex09 Serializing an integer into a std::vector with FixedSize
\include 09_ser_raw_vector_fsize.cpp
\section Ex10 Deserializing an integer from a dynamically-sized std::vector with DynSize
\include 10_deser_raw_vector_dynsize.cpp
\section Ex11 Serializing an integer into a std::vector with InfSize
\include 11_ser_raw_vector_infsize.cpp
\section Ex12 Using return codes to process errors
\include 12_errorcode.cpp
\section Ex13 Defining a simple struct for serialization
\include 13_struct.cpp
\section Ex14 Annotating a struct with attributes
\include 14_struct_attr.cpp
\section Ex15 Annotating a struct member with attributes
\include 15_struct_memattr.cpp
\section Ex16 Defining and annotating a struct member in one step
\include 16_structmem_compact.cpp
\section Ex17 Defining and annotating all struct members in one step
\include 17_struct_compact.cpp
\section Ex18 Annotating a struct without modifying its definition
\include 18_struct_ext.cpp
\section Ex19 Annotating a struct without modifying its definition in a compact fashion
\include 19_struct_ext_compact.cpp
\section Ex20 Serializing an integer that is not a multiple of a byte in different byte orders
\include 20_byte_order.cpp
\section Ex21 Serializing a signed integer with different sign formats
\include 21_sign_format.cpp
\section Ex22 Setting integer width to convert RGB565 color values
\include 22_integer_width.cpp
\section Ex23 Using padding bits to skip dummy data in the raw stream
\include 23_integer_padding.cpp
\section Ex24 Convert a file's byte order by using input/output iterators
\include 24_stream_iterators.cpp
\section Ex25 Convert a file's byte order by using custom input/output iterators
\include 25_user_iterator1.cpp
\section Ex26 Deserializing a vector of integers from an array.
\include 26_deser_vector.cpp
\section Ex27 Defining dynamic data structures with Dyn::Size
\include 27_dynsize.cpp
\section Ex28 Defining optional data structures with Dyn::Optional
\include 28_optional.cpp
\section Ex29 Defining hook funktions to be called before/after (de)serialization
\include 29_hook.cpp
\section ExC Using µSer in C projects
\subsection ExC_Hdr Common header file for data structures
\include C/packet.h
\subsection ExC_Ser Implementation of the C++ serialization functions
\include C/serialize.cpp
\subsection ExC_main Calling serialization functions from C code
\include C/main.c
