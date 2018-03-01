/**
 * \file main.c
 * \brief Example for using µSer from C code by using a common data structure for C and C++
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

#include <stdio.h>
#include <inttypes.h>
#include "packet.h"


int main () {
	// Define a packet instance and fill it with some data
	PacketB pk;
	pk.N = 2;
	pk.packets [0].a = 0xDEADBEEF;
	pk.packets [0].b = 0xAA55;
	pk.packets [0].c = -42;

	pk.packets [1].a = 0xC0FEBABE;
	pk.packets [1].b = 0x1234;
	pk.packets [1].c = 35;

	// Define an array to receive the raw data
	uint8_t raw [15];
	// Serialize the packet into the raw data
	uSer_ErrorCode ec = serializePacketB (raw, &pk, sizeof (raw) /* uint8_t always has size 1 */);
	// Check for errors
	if (ec != uSer_EOK) {
		fprintf (stderr, "Serialization error: %s\n", uSer_getErrorMessage (ec));
		return 1;
	} else {
		// Print the raw data
		puts ("Serialization result:");

		for (size_t i = 0; i < sizeof(raw); ++i) {
			printf ("%02x, ", raw [i]);
		}
		puts ("");
	}

	// Deserialize the raw data back into the struct
	ec = deserializePacketB (raw, &pk, sizeof (raw));
	// Check for errors
	if (ec != uSer_EOK) {
		fprintf (stderr, "Deserialization error: %s\n", uSer_getErrorMessage (ec));
		return 1;
	} else {
		// Print the deserialized data structure
		puts ("Deserialization result:");

		printf ("pk.N=%" PRIu8 "\n", pk.N);

		for (size_t i = 0; i < pk.N; ++i) {
			printf ("pk.packets[%zd].a=%" PRIx32 ", .b=%" PRIx16 ", .c=%" PRId8 "\n", i, pk.packets [i].a, pk.packets [i].b, pk.packets [i].c);
		}
	}
}
