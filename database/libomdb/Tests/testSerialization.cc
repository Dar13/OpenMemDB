/**
 * Tests the serialization_helper methods
 */

#include <cassert>
#include <cstring>
#include <iostream>

#include "../../include/util/serialization_helper.h"
#include "../../include/util/libomdb.h"

int main (int argc, char** argv) {
  // Make a packet, serialize it, then deserialize it and check for
  // expected results
  ConnectionPacket connectionPacket;
  memcpy(connectionPacket.name, "db_name", sizeof("db_name"));
  connectionPacket.type = PacketType::COMMAND;

  char* serializedConnectionPacket = SerializeConnectionPacket(connectionPacket);
  std::cout << serializedConnectionPacket << std::endl;

  ConnectionPacket deserializedConnectionPacket =
          DeserializeConnectionPacket(serializedConnectionPacket);

  assert(deserializedConnectionPacket.type == PacketType::COMMAND);

  assert(strcmp(deserializedConnectionPacket.name, "db_name") == 0);

  return 0;
}