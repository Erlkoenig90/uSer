/**
 * \file 06_be_raw.cpp
 * \brief Pre-Processing raw data with swapped bytes
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
#include <tuple>
#include <uSer.hh>

int main () {
	// Define an array to receive the raw binary data.
	std::uint8_t raw [4];
	// Define the data to be serialized.
	std::tuple<std::uint16_t, std::uint8_t, std::uint8_t> x { 0x4554, 0x53, 0x54 };
	// Perform the actual serialization.
	uSer::serialize (raw, x);

	// Simulate a communication channel which combines the 8-Bit-Integers into 16-Bit-Integers
	// in a big endian way:
	std::uint16_t raw2 [2];
	for (std::size_t i = 0; i < sizeof(raw)/2 /* uint8_t always has size 1 */; ++i) {
		raw2[i] = static_cast<std::uint16_t> ((uint16_t { raw[i*2] } << 8) | raw [i*2+1]);
	}

	// Turn the swapped 16-Bit-Words back into a sequence of words where the least significant bit comes first
	std::uint16_t raw3 [2];
	uSer::deserialize<uSer::ByteOrder::BE> (raw2, raw3);

	// Actually deserialize the data
	uSer::deserialize (raw3, x);

	// Write the data to standard output. We need to convert the uint8_t elements into integers,
	// because else they will be interpreted as individual characters.
	std::cout << std::hex << "0x" << std::get<0> (x) << ", " << "0x" << int { std::get<1> (x) } << ", " << "0x" << int { std::get<2> (x) } << std::endl;
}
