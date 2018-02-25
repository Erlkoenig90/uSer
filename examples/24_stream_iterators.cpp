/**
 * \file 24_stream_iterators.cpp
 * \brief Convert a file's byte order by using input/output iterators
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
#include <ios>
#include <fstream>
#include <iterator>
#include <sstream>
#define USER_EXCEPTIONS
#include <uSer.hh>

int main () {
	try {
		// Open input file
		std::ifstream input ("rawdata.be", std::ios::binary);
		if (!input)
			throw std::runtime_error ("File could not be opened");

		// Determine file size
		input.seekg (0, std::ios::end);
		std::size_t fSize = static_cast<std::size_t> (input.tellg ());
		input.seekg (0, std::ios::beg);

		std::uint32_t x;
		// Read a 32-Bit-Integer as big endian
		uSer::deserialize<uSer::ByteOrder::BE> (std::istream_iterator<std::uint8_t> (input), x, fSize);

		// Open output file
		std::ofstream output ("rawdata.le", std::ios::binary);
		output.exceptions (std::ofstream::failbit);

		// Write the integer as little endian
		uSer::serialize<uSer::RawInfo<std::uint8_t>, uSer::ByteOrder::LE> (std::ostream_iterator<std::uint8_t> (output), x, uSer::infSize);
	} catch (const std::exception& ex) {
		// Handle errors
		std::cerr << "Error: " << ex.what () << std::endl;
	}
}
