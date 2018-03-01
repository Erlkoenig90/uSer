/**
 * \file packet.h
 * \brief Common header file for C and C++ to declare serializable data structures ("packets")
 * \author Niklas Gürtler
 * \copyright (C) 2018 Niklas Gürtler
 *
 * \copyright Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * \copyright The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * \copyright THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * \copyright Except as contained in this notice, the names of the authors or
 * their institutions shall not be used in advertising or otherwise to
 * promote the sale, use or other dealings in this Software without
 * prior written authorization from the authors.
 */

#include <uSer.hh>

// Define data structures compatible with both C and C++.

// Define a simple struct containing integers
struct PacketA {
	// Begin struct definition
	USER_STRUCT(PacketA, uSer::AttrNone)

	uint32_t a;
	// Encode a in Big-Endian in the raw data
	USER_MEM_ANNOT(a, uSer::ByteOrder::BE)

	uint16_t b;
	int8_t c;
	// Encode c in signed-magnitude format in the raw data
	USER_MEM_ANNOT(c, uSer::SignFormat::SignedMagnitude)

	// List members
	USER_ENUM_MEM (a, b, c)
};

// Define a struct containing a dynamic data structure. Use the "typedef struct"-Trick to make usage more convenient.
typedef struct PacketB_ {
	// Begin struct definition
	USER_STRUCT (PacketB_, uSer::AttrNone)

	uint8_t N;
	struct PacketA packets [8];
	// Let N denote the size of the arra packets.
	USER_MEM_ANNOT (packets, uSer::Dyn::Size<&PacketB_::N>)

	USER_ENUM_MEM (N, packets)
} PacketB;

// Declare functions for (de)serialization of the data structure. These will be implemented as C++.

USER_EXTERN_C uSer_ErrorCode serializePacketB (uint8_t* raw, const PacketB* pk, size_t bufferSize);
USER_EXTERN_C uSer_ErrorCode deserializePacketB (const uint8_t* raw, PacketB* pk, size_t bufferSize);
