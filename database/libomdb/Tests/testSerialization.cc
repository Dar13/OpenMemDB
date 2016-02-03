/**
 * Tests the serialization_helper methods
 */

#include <cassert>
#include <cstring>
#include <iostream>

#include "../../include/util/serialization_helper.h"
#include "../../include/util/libomdb.h"
#include "../result.h"

bool testCommandPacketSerialization();
bool testConnectionPacketSerialization();
bool testResultPacketSerialization();
bool testResultMetaDataPacketSerialization();


int main (int argc, char** argv) {

  bool commandPassed = testCommandPacketSerialization();
  bool connectionPassed = testConnectionPacketSerialization();
  bool resultMetaDataPassed = testResultMetaDataPacketSerialization();
  std::cout << "Command Serialization Passed: " << commandPassed << std::endl;
  std::cout << "Connection Serialization Passed: " << connectionPassed << std::endl;
  std::cout << "ResultMetaData Serialization Passed: " << resultMetaDataPassed <<std::endl;

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
  std::string TAG("ResultMetaDataPacket Test: ");
  ResultMetaDataPacket resultMetaDataPacket;
  resultMetaDataPacket.type = PacketType::RESULT_METADATA;
  resultMetaDataPacket.status = ResultStatus::OK;
  libomdb::MetaDataColumn metaDataColumns[5];
  for (int i = 0; i < 5; ++i) {
    libomdb::MetaDataColumn metaDataColumn;
    metaDataColumn.label = "test label"+i;
    metaDataColumn.sqlType = 1;
    metaDataColumns[i] = metaDataColumn;
  }
  resultMetaDataPacket.numColumns = 5;
  resultMetaDataPacket.terminator = THE_TERMINATOR;

  // Serialize packet
  char* serializedPacket = SerializeResultMetaDataPacket(resultMetaDataPacket);
  std::cout << serializedPacket << std::endl;

  // Deserialize packet
  ResultMetaDataPacket deserializedPacket = DeserializeResultMetaDataPacket(serializedPacket);
  std::cout << TAG << "Num columns: "<< deserializedPacket.numColumns << std::endl;

}
