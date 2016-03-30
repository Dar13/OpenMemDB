/* TODO Copyright header */

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
