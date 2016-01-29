/**
 * Tests the serialization_helper methods
 */

#include <cassert>
#include <cstring>
#include <iostream>

#include "../../include/util/serialization_helper.h"
#include "../../include/util/libomdb.h"

bool testCommandPacketSerialization();
bool testConnectionPacketSerialization();
bool testResultPacketSerialization();
bool testResultMetaDataPacketSerialization();


int main (int argc, char** argv) {

  bool commandPassed = testCommandPacketSerialization();
  bool connectionPassed = testConnectionPacketSerialization();
  std::cout << "Command Serialization Passed: " << commandPassed << std::endl;
  std::cout << "Connection Serialization Passed: " << connectionPassed << std::endl;

  return 0;
}

bool testCommandPacketSerialization() {
  CommandPacket commandPacket;
  memcpy(commandPacket.message, "command message", sizeof("command message"));
  commandPacket.commandType = CommandType::DB_COMMAND;
  std::cout << "Name of db: " << commandPacket.message << std::endl;
  char* serializedCommandPacket = SerializeCommandPacket(commandPacket);
  CommandPacket deserializedCommandPacket =
      DeserializeCommandPacket(serializedCommandPacket);
  assert(deserializedCommandPacket.commandType == CommandType::DB_COMMAND);

  assert(strcmp(deserializedCommandPacket.message, "command message") == 0);
}

bool testConnectionPacketSerialization() {
  // Make a packet, serialize it, then deserialize it and check for
  // expected results
  ConnectionPacket connectionPacket;
  memcpy(connectionPacket.name, "db_name", sizeof("db_name"));
  connectionPacket.type = PacketType::COMMAND; // Because there is not CONNECTION type
  std::cout << "Name of db: " << connectionPacket.name << std::endl;
  char* serializedConnectionPacket = SerializeConnectionPacket(connectionPacket);
  std::cout << serializedConnectionPacket << std::endl;

  ConnectionPacket deserializedConnectionPacket =
      DeserializeConnectionPacket(serializedConnectionPacket);

  assert(deserializedConnectionPacket.type == PacketType::COMMAND);

  assert(strcmp(deserializedConnectionPacket.name, "db_name") == 0);
}

bool testResultPacketSerialization() {

}

bool testResultMetaDataPacketSerialization() {

}
