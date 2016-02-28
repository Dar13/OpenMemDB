/*
The MIT License (MIT)
Copyright (c) 2015 University of Central Florida's Computer Software Engineering
Scalable & Secure Systems (CSE - S3) Lab
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <cstring>
#include <cstdio>
#include <iostream>

#include "include/util/libomdb.h"

/*****************************************************************
 * Helper functions
 *****************************************************************/
/*void deserializeResultColumnsAndTerminator(ResultMetaDataPacket* packet,
                                           const char* serializedPacket,
                                           uint index) {
  uint32_t col = 0;
  ResultColumn columns[MAX_NUM_COLUMNS];
  while (serializedPacket[index] != THE_TERMINATOR) {
    ResultColumn column;
    memcpy(column.name, &serializedPacket[index], COL_NAME_LEN);
    index += COL_NAME_LEN;
    uint16_t* type = new uint16_t;
    memcpy(type, &serializedPacket[index], sizeof(uint16_t));
    column.type = *type;
    delete(type);
    columns[col] = column;
    col++;
    index += (COL_NAME_LEN + sizeof(uint16_t));
  }

  //packet->columns = columns;
  memcpy(packet->columns, columns, sizeof(columns));
}*/


// Need to map to enums from strings.
CommandType mapStringToCommandType(char* type) {
  CommandType commandType = static_cast<CommandType>(type[0]);
  //CommandType commandType = static_cast<CommandType>(std::stoi(typeString));
  if (commandType == CommandType::DB_COMMAND || commandType == CommandType::SQL_STATEMENT) {
    return commandType;
  }

  return CommandType::INVALID_COMMAND;
}

PacketType mapStringToPacketType(char* type) {
  if (strcmp(type, "NONE") == 0) {
    return PacketType::NONE;
  } else if (strcmp(type, "COMMAND")) {
    return PacketType::COMMAND;
  } else if (strcmp(type, "RESULT_METADATA") == 0) {
    return PacketType::RESULT_METADATA;
  } else if (strcmp(type, "RESULT_DATA") == 0) {
    return PacketType::RESULT_DATA;
  }

  return PacketType::INVALID_PACKET;
}

/*****************************************************************
 * Serialization functions
 *****************************************************************/

char* SerializeCommandPacket(CommandPacket packet) {
  // Take command packet and turn into char*
  //char serializedCommandPacket[sizeof(packet)];
  char* serializedCommandPacket = new char[sizeof(packet)];
  memcpy(serializedCommandPacket, &packet, sizeof(packet));
  return serializedCommandPacket;
}


char* SerializeConnectionPacket(ConnectionPacket packet){
  //char serializedConnectionPacket[sizeof(packet)];
  char* serializedConnectionPacket = new char[sizeof(packet)];
  memcpy(serializedConnectionPacket, &packet, sizeof(packet));
  return serializedConnectionPacket;
}

char* SerializeResultMetaDataPacket(ResultMetaDataPacket packet){
  char* buffer = new (std::nothrow) char[sizeof(ResultMetaDataPacket)];
  if(buffer == nullptr) {
    printf("ERROR: Failed to allocate memory for packet buffer!\n");
    return nullptr;
  }

  memcpy(buffer, &packet, sizeof(ResultMetaDataPacket));
  return buffer;
}


char* SerializeResultPacket(ResultPacket packet){
  size_t size = 8 + packet.resultSize + 1;
  char* buffer = new (std::nothrow) char[size];
  memset(buffer, 0, size);
  memcpy(buffer, &packet, sizeof(char) * 8);
  memcpy(buffer + 8, packet.data, packet.resultSize);
  buffer[size - 1] = THE_TERMINATOR;
  return buffer;
}

/*****************************************************************
 * Deserialization functions
 *****************************************************************/


CommandPacket DeserializeCommandPacket(char* serializedPacket){
  CommandPacket commandPacket;
  char* type = new char[sizeof(CommandType)];
  memcpy(type, &serializedPacket[0], sizeof(CommandType));
  memcpy(commandPacket.message, &serializedPacket[1], DB_NAME_LEN);
  commandPacket.commandType = mapStringToCommandType(type);
  delete[] type;
  return commandPacket;
}


ConnectionPacket DeserializeConnectionPacket(const char* serializedPacket){
  ConnectionPacket connectionPacket;
  char* type = new char[sizeof(ConnectionPacket)];
  memcpy(type, &serializedPacket[0], sizeof(uint8_t));
  memcpy(connectionPacket.name, &serializedPacket[1], DB_NAME_LEN);
  connectionPacket.type = mapStringToPacketType(type);
  delete[] type;
  //connectionPacket.name[DB_NAME_LEN - 1] = 0; // I think neil said not necessary
  return connectionPacket;
}


ResultMetaDataPacket DeserializeResultMetaDataPacket(char* serializedPacket){
  ResultMetaDataPacket metaDataPacket;
  memcpy(&metaDataPacket, serializedPacket, sizeof(ResultMetaDataPacket));
  return metaDataPacket;
}


ResultPacket DeserializeResultPacket(char* serializedPacket){
  ResultPacket packet;
  // Place the first 8 bytes into packet.
  memcpy(&packet, serializedPacket, 8);
  packet.data = new uint64_t[packet.resultSize/sizeof(uint64_t)];
  memset(packet.data, 0, sizeof(uint64_t) * (packet.resultSize/sizeof(uint64_t)));
  memcpy(packet.data, &serializedPacket[8], packet.resultSize);
  packet.terminator = serializedPacket[8 + packet.resultSize];
  return packet;
}
