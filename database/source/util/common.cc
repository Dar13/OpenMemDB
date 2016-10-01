/* Copyright (c) 2016 Neil Moore, Jason Stavrinaky, Micheal McGee, Robert Medina
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <cstring>

#include "util/common.h"

/**
 *	\brief Checks the given string for ISO 8601 compliance
 */
bool checkDateFormat(std::string str)
{
	if(str.length() < 10) { return false; }

	for(int i = 0; i < 10; i++)
	{
		switch(i)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 5:
			case 6:
			case 8:
			case 9:
				if(isdigit(str[i]) == 0) { return false; }
				break;
			case 4:
			case 7:
				if(str[i] != '-') { return false; }
			default:
				return false;
		}
	}

	return true;
}

void serializeMetaDataPacket(ResultMetaDataPacket packet, char* out_buffer)
{
    // Assume the buffer is long enough for this
    std::memset(out_buffer, 0, sizeof(ResultMetaDataPacket));
    std::memcpy(out_buffer, &packet, sizeof(ResultMetaDataPacket));
}

void serializeResultPacket(ResultPacket packet, char* out_buffer)
{
    std::memcpy(out_buffer, &packet, 9);
    if(packet.data == nullptr)
    {
        std::memset(out_buffer + 10, packet.terminator, sizeof(char));
    }
    else
    {
        std::memcpy(out_buffer + 10, packet.data, packet.resultSize);
        out_buffer[9 + packet.resultSize + 1] = packet.terminator;
    }
}
