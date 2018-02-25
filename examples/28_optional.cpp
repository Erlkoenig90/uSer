/**
 * \file 28_optional.cpp
 * \brief Defining optional data structures with Dyn::Optional
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
#define USER_EXCEPTIONS
#include <uSer.hh>

struct A;
bool g_getSize (const struct A&);

// Functional for determining the existence (class with operator () overloaded)
struct Functional {
	bool operator () (const struct A&);
} functional;

// Define a serializable struct
struct A {
	// Member function for determining whether an object is present
		bool m_getSize () const {
		return N2;
	}

	// Begin declaration
	USER_STRUCT (A, uSer::AttrNone)

	// 1 if v1 exists, 0 else
	std::uint8_t N1;

	// Define an optional object
	std::uint8_t v1;
	// Define the presence of an object to be optional using a member variable
	USER_MEM_ANNOT (v1, uSer::Dyn::Optional<&A::N1>)

	// 1 if v2 exists, 0 else
	std::uint8_t N2;

	// Define an optional object
	std::uint8_t v2;
	// Define the presence of an object to be optional using a member function
	USER_MEM_ANNOT (v2, uSer::Dyn::Optional<&A::m_getSize>)

	// 1 if v3 exists, 0 else
	std::uint8_t N3;

	// Define an optional object
	std::uint8_t v3;
	// Define the presence of an object to be optional using a free function
	USER_MEM_ANNOT (v3, uSer::Dyn::Optional<&g_getSize>)

	// 1 if v4 exists, 0 else
	std::uint8_t N4;

	// Define an optional object
	std::uint8_t v4;
	// Define the presence of an object to be optional using a functional (class with operator () overloaded)
	USER_MEM_ANNOT (v4, uSer::Dyn::Optional<&functional>)

	// Make members known to µSer
	USER_ENUM_MEM (N1, v1, N2, v2, N3, v3, N4, v4)
};

// Free function for determining the presence
bool g_getSize (const struct A& a) {
	return a.N3 != 0;
}

bool Functional::operator () (const struct A& a) {
	return a.N4 != 0;
}


int main () {
	// Define an array with binary data
	const std::uint8_t raw [6] = {	0x01, 0x01,
									0x00,
									0x01, 0x2A,
									0x00 };
	// Define a struct to receive the deserialized data
	A x;
	try {
		// Perform the actual deserialization.
		uSer::deserialize (raw, x);

		// Write the read data to standard output.

		if (x.N1) std::cout << "v1: " << std::hex << int (x.v1) << std::endl;
		if (x.N2) std::cout << "v2: " << std::hex << int (x.v2) << std::endl;
		if (x.N3) std::cout << "v3: " << std::hex << int (x.v3) << std::endl;
		if (x.N4) std::cout << "v4: " << std::hex << int (x.v4) << std::endl;
	} catch (const uSer::Exception& ex) {
		// In case of error, print it
		std::cerr << "uSer Error: " << ex.what () << std::endl;
	}
}
