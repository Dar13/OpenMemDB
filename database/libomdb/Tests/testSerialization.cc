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
  bool resultPassed = testResultPacketSerialization();
  std::cout << "Command Serialization Passed: " << commandPassed << std::endl;
  std::cout << "Connection Serialization Passed: " << connectionPassed << std::endl;
  std::cout << "ResultMetaData Serialization Passed: " << resultMetaDataPassed <<std::endl;
  std::cout << "Result Serialization Passed: " << resultPassed << std::endl;
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

  return true;
}

bool testConnectionPacketSerialization() {
  // Make a packet, serialize it, then deserialize it and check for
  // expected results
  ConnectionPacket connectionPacket;
  strcpy(connectionPacket.name, "db_name");
  connectionPacket.type = PacketType::COMMAND; // Because there is not CONNECTION type
  std::cout << "Name of db: " << connectionPacket.name << std::endl;
  char* serializedConnectionPacket = SerializeConnectionPacket(connectionPacket);
  std::cout << serializedConnectionPacket << std::endl;

  ConnectionPacket deserializedConnectionPacket =
      DeserializeConnectionPacket(serializedConnectionPacket);

  assert(deserializedConnectionPacket.type == PacketType::COMMAND);

  assert(strcmp(deserializedConnectionPacket.name, "db_name") == 0);

  delete serializedConnectionPacket;

  return true;
}

bool testResultPacketSerialization() {
  // Create result packet
  ResultPacket resultPacket;
  resultPacket.status = ResultStatus::OK;
  resultPacket.resultSize = 64;
  resultPacket.rowLen = 4;
  resultPacket.terminator = THE_TERMINATOR;
  // Put data in the packet
  uint64_t* resultPacketData = new uint64_t[4];
  for (int i = 0; i < resultPacket.resultSize/8; ++i) { // 2 rows
    resultPacketData[i] = (uint64_t)i;  
  }
  resultPacket.data = resultPacketData;
  // Serialize the result packet
  char* serializedResultPacket = SerializeResultPacket(resultPacket);

  // Deserialize result packet
  ResultPacket deserializedResultPacket = DeserializeResultPacket(serializedResultPacket);

  // Assert some values
  std::cout << "Result Status: " << (int)deserializedResultPacket.status << std::endl;
  assert(deserializedResultPacket.status == ResultStatus::OK);
  assert(deserializedResultPacket.resultSize == 64);
  assert(deserializedResultPacket.rowLen == 4);
  assert(deserializedResultPacket.terminator == THE_TERMINATOR);

  // Print some of the result data
  int numData = deserializedResultPacket.resultSize / sizeof(uint64_t);
  for (int i = 0; i < numData; ++i) {
    std::cout << deserializedResultPacket.data[i] << std::endl;
  }
  return true;
}

bool testResultMetaDataPacketSerialization() {
  std::string TAG("ResultMetaDataPacket Test: ");
  ResultMetaDataPacket resultMetaDataPacket;
  memset(&resultMetaDataPacket, 0, sizeof(resultMetaDataPacket));
  resultMetaDataPacket.type = PacketType::RESULT_METADATA;
  resultMetaDataPacket.status = ResultStatus::OK;

  for (int i = 0; i < 5; ++i) {
    std::string label = "test label" + std::to_string(i);

    resultMetaDataPacket.columns[i].type = 1;
    size_t len = (label.length() > COL_NAME_LEN) ? COL_NAME_LEN : label.length();
    memcpy(resultMetaDataPacket.columns[i].name, label.c_str(), len);
  }

  resultMetaDataPacket.numColumns = 5;
  resultMetaDataPacket.terminator = THE_TERMINATOR;

  // Serialize packet
  char* serializedPacket = SerializeResultMetaDataPacket(resultMetaDataPacket);
  std::cout << serializedPacket << std::endl;

  // Deserialize packet
  ResultMetaDataPacket deserializedPacket = DeserializeResultMetaDataPacket(serializedPacket);
  std::cout << TAG << "Num columns: "<< deserializedPacket.numColumns << std::endl;

  assert(deserializedPacket.type == PacketType::RESULT_METADATA);
  assert(deserializedPacket.numColumns == 5);
  for (ResultColumn col: deserializedPacket.columns) {
    std::cout << "Column name:" << col.name << std::endl;
    std::cout << "Column type:" << col.type << std::endl;
  }

  delete serializedPacket;

  return true;
}
