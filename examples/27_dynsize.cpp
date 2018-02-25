/**
 * \file 27_dynsize.cpp
 * \brief Defining dynamic data structures with Dyn::Size
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
std::size_t g_getSize (const struct A&);

// Functional for determining the dynamic size (class with operator () overloaded)
struct Functional {
	std::size_t operator () (const struct A&);
} functional;

// Define a serializable struct
struct A {
	// Member function for determining the dynamic size
	std::size_t m_getSize () const {
		return N2;
	}

	// Begin declaration
	USER_STRUCT (A, uSer::AttrNone)

	// Size of array1
	std::uint16_t N1;

	// Define an array
	std::uint8_t array1 [20];
	// Define the array size dynamically using a member variable
	USER_MEM_ANNOT (array1, uSer::Dyn::Size<&A::N1>)

	std::uint16_t N2;

	// Define an array
	std::uint8_t array2 [20];
	// Define the array size dynamically using a member function
	USER_MEM_ANNOT (array2, uSer::Dyn::Size<&A::m_getSize>)

	std::uint16_t N3;

	// Define an array
	std::uint8_t array3 [20];
	// Define the array size dynamically using a free function
	USER_MEM_ANNOT (array3, uSer::Dyn::Size<&g_getSize>)

	std::uint16_t N4;

	// Define an array
	std::uint8_t array4 [20];
	// Define the array size dynamically using a functional (class with operator () overloaded)
	USER_MEM_ANNOT (array4, uSer::Dyn::Size<&functional>)

	// Make members known to µSer
	USER_ENUM_MEM (N1, array1, N2, array2, N3, array3, N4, array4)
};

// Free function for determining the dynamic size
std::size_t g_getSize (const struct A& a) {
	return a.N3;
}

std::size_t Functional::operator () (const struct A& a) {
	return a.N4;
}


int main () {
	// Define an array with binary data
	const std::uint8_t raw [26] = {	0x03, 0x00, 0x01, 0x02, 0x03,
										0x04, 0x00, 0x01, 0x02, 0x03, 0x04,
										0x05, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
										0x06, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
	// Define a struct to receive the deserialized data
	A x;
	try {
		// Perform the actual deserialization.
		uSer::deserialize (raw, x);

		// Write the read data to standard output.

		std::copy (x.array1, x.array1 + x.N1, std::ostream_iterator<int> (std::cout, ", ")); std::cout << std::endl;
		std::copy (x.array2, x.array2 + x.N2, std::ostream_iterator<int> (std::cout, ", ")); std::cout << std::endl;
		std::copy (x.array3, x.array3 + x.N3, std::ostream_iterator<int> (std::cout, ", ")); std::cout << std::endl;
		std::copy (x.array4, x.array4 + x.N4, std::ostream_iterator<int> (std::cout, ", ")); std::cout << std::endl;
	} catch (const uSer::Exception& ex) {
		// In case of error, print it
		std::cerr << "uSer Error: " << ex.what () << std::endl;
	}
}
