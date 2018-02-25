/**
 * \file 26_deser_vector.cpp
 * \brief Deserializing a vector of integers from an array.
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
#include <vector>
#define USER_EXCEPTIONS
#include <uSer.hh>

int main () {
	// Define an array with the raw binary data.
	const std::uint8_t raw [4] { 0x54, 0x45, 0x53, 0x54 };
	// Define a vector to receive the deserialized data and allocate it to the desired size.
	std::vector<std::uint16_t> x (2);
	try {
		// Perform the actual deserialization.
		uSer::deserialize (raw, x);
		// Write the data to standard output.
		std::cout << std::hex << "0x" << x [0] << ", 0x" << x [1] << std::endl;
	} catch (const uSer::Exception& ex) {
		// In case of error, print it
		std::cerr << "uSer Error: " << ex.what () << std::endl;
	}
}
