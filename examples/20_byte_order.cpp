/**
 * \file 20_byte_order.cpp
 * \brief Serializing an integer that is not a multiple of a byte in different byte orders
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

int main () {
	// Define an array to receive the raw binary data.
	std::uint8_t raw [2];
	// Define the data to be serialized.
	std::uint16_t x = 0x765;
	// Serialize as little endian
	uSer::serialize<uSer::ByteOrder::LE, uSer::Width<11>> (raw, x);

	// Write the binary data to standard output.
	for (std::uint8_t r : raw)
		std::cout << std::hex << std::setw(2) << "0x" << int { r } << ", ";
	std::cout << std::endl;

	// Serialize as big endian
	uSer::serialize<uSer::ByteOrder::BE, uSer::Width<11>> (raw, x);

	// Write the binary data to standard output.
	for (std::uint8_t r : raw)
		std::cout << std::hex << std::setw(2) << "0x" << int { r } << ", ";
	std::cout << std::endl;
}
