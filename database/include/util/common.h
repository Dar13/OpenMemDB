/* TODO Copyright header */

#ifndef UTIL_COMMON_H
#define UTIL_COMMON_H

#include <string>

#include "packets.h"

bool checkDateFormat(std::string str);

void serializeMetaDataPacket(ResultMetaDataPacket packet, char* out_buffer);
void serializeResultPacket(ResultPacket packet, char* out_buffer);

#endif
