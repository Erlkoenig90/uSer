/**
 * \file 18_struct_ext.cpp
 * \brief Annotating a struct without modifying its definition.
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
#include <cstdint>
#include <iostream>
#include <uSer.hh>

namespace N {
	// Normal definition of a struct
	struct A {
		// Define arbitrary members
		std::uint16_t a, b;
		std::uint8_t c;
		std::uint32_t d;
	};
}

// These macro calls need to be in the global scope.

// Optional: Define attributes for the whole struct.
USER_EXT_ANNOT(N::A, uSer::ByteOrder::LE)

// Optional: Annotate a struct member
USER_EXT_MEM_ANNOT(N::A, d, uSer::ByteOrder::BE)

// List all struct members
USER_EXT_ENUM_MEM(N::A, a, b, c, d)

int main () {
	// Define an array to receive the raw binary data.
	std::uint8_t raw [9];
	// Define the data to be serialized.
	N::A x { 0x1234, 0x4321, 0xAF, 0xDEADBEEF };
	// Perform the actual serialization.
	uSer::serialize (raw, x);

	// Write the binary data to standard output.
	for (std::uint8_t r : raw)
		std::cout << std::hex << std::setw(2) << "0x" << int { r } << ", ";
	std::cout << std::endl;
}
