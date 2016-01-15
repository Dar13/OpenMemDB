//
// Created by mike on 1/14/16.
//

#ifndef OPENMEMDB_LIBOMDB_H
#define OPENMEMDB_LIBOMDB_H

#include <string>

#define MESSAGE_SIZE 300

const uint8_t THE_TERMINATOR = 0xFF;

/**
 * Enum used to describe the type of packet being sent
 */
enum CommandType {
  SQL_STATEMENT = 0x01,
  DB_COMMAND = 0x02
};

typedef struct NetworkPacket {
  CommandType commandType;
  char message[MESSAGE_SIZE];
  uint8_t terminator;
} NetworkPacket;

#endif //OPENMEMDB_LIBOMDB_H
