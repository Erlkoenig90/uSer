/**
 * \file 22_integer_width.cpp
 * \brief Setting integer width to convert RGB565 color values
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

// Define a serializable struct for storing colors in RGB565 format.
struct Color {
	USER_STRUCT (Color, uSer::AttrNone)

	// Define color components.
	std::uint8_t r, g, b;

	// Explicitly set the sizes of the integers
	USER_MEM_ANNOT(r, uSer::Width<5>)
	USER_MEM_ANNOT(g, uSer::Width<6>)
	USER_MEM_ANNOT(b, uSer::Width<5>)

	// Make members known to µSer
	USER_ENUM_MEM (r, g, b)
};

int main () {
	// Store the binary data in a 16bit-Integer
	std::uint16_t raw [1];
	// Define a color to be serialized.
	Color c1 { 24, 55, 12 };
	// Convert RGB565-Color into 16bit-Value
	uSer::serialize (raw, c1);

	// Output the raw value
	std::cout << std::hex << std::setw(4) << "0x" << raw[0] << std::endl;
}
