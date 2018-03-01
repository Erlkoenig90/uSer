/**
 * \file 29_hook.cpp
 * \brief Defining hook funktions to be called before/after (de)serialization
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

void serPost (const uint8_t&, const A&);

// Functional to be called before deserialization
struct DeSerPre {
	void operator () (uint8_t&, A&);
} deSerPre;

// Define a serializable struct
struct A {
	// Member function to be called before serialization
	void serPre (const uint8_t& a) const {
		std::cout << "SerPre\n";
		// Simulate an error condition
		if (a != 42)
			throw uSer::Exception (uSer_EHOOK);
	}

	// Member function to be called after deserialization
	uSer_ErrorCode deSerPost () {
		std::cout << "DeSerPost\n";
		// Simulate an error condition
		return a == 42 ? uSer_EOK : uSer_EHOOK;
	}

	// Begin declaration. Define hook to be called after deserialization of the whole struct.
	USER_STRUCT (A, uSer::Hook::DeSerPost<&A::deSerPost>)

	// Define some members
	std::uint8_t a, b, c;

	// Define hooks to be called before/after (de)serialization of the individual members
	USER_MEM_ANNOT(a, uSer::Hook::SerPre<&A::serPre>)
	USER_MEM_ANNOT(b, uSer::Hook::SerPost<&serPost>)
	USER_MEM_ANNOT(c, uSer::Hook::DeSerPre<&deSerPre>)

	// Make members known to µSer
	USER_ENUM_MEM (a, b, c)
};

// Hook function to be called after serialization
void serPost (const uint8_t&, const A&) {
	std::cout << "SerPost\n";
}

void DeSerPre::operator () (uint8_t&, A&) {
	std::cout << "DeSerPre\n";
}

int main () {
	// Define an array to receive the raw binary data.
	std::uint8_t raw [3];
	// Define the data to be serialized.
	A x { 42, 2, 3 };
	try {
		// Serialize & deserialize
		uSer::serialize (raw, x);
		uSer::deserialize (raw, x);
	} catch (const uSer::Exception& ex) {
		// In case of error, print it
		std::cerr << "uSer Error: " << ex.what () << std::endl;
	}
}
