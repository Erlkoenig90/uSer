/**
 * \file serialize.cpp
 * \brief Implement in C++ the serialization functions for the data structures.
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

#include "packet.h"

USER_EXTERN_C uSer_ErrorCode serializePacketB (uint8_t* raw, const PacketB* pk, size_t bufferSize) {
	// Just call the µSer serialization function
	return uSer::serialize (raw, *pk, bufferSize);
}

USER_EXTERN_C uSer_ErrorCode deserializePacketB (const uint8_t* raw, PacketB* pk, size_t bufferSize) {
	// Just call the µSer deserialization function
	return uSer::deserialize (raw, *pk, bufferSize);
}
