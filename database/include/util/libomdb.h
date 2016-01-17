//
// Created by mike on 1/14/16.
//

#ifndef OPENMEMDB_LIBOMDB_H
#define OPENMEMDB_LIBOMDB_H

#include <string>

#define MESSAGE_SIZE 300

const uint8_t THE_TERMINATOR = 0xFF;
const uint32_t DB_NAME_LEN = 50;
const uint32_t COL_NAME_LEN = 25;
const uint32_t MAX_NUM_COLUMNS = 20;

/**
 * Enum used to describe the type of packet being sent
 */
enum class CommandType: uint8_t {
  SQL_STATEMENT = 0x01,
  DB_COMMAND = 0x02
};


enum class PacketType: uint8_t {
  NONE = 0,
  COMMAND,
  RESULT_METADATA,
  RESULT_DATA
};


enum class ResultStatus: uint8_t {
  OK = 0,
  ERROR,
  ERROR_SYNTAX
};


struct CommandPacket {
  CommandType commandType;  // The type of command requested to be executed: SQL_STATEMENT or DB_COMMAND
  char message[MESSAGE_SIZE];
  uint8_t terminator;
};


struct ConnectionPacket {
  PacketType type;
  char name[DB_NAME_LEN]; // The name of the database connected to.
};


struct ResultColumn {
  char name[COL_NAME_LEN];  // The column name
  uint16_t type;            // The SQL type of the column
};


struct ResultMetaDataPacket {
  PacketType type;
  ResultStatus status;
  uint32_t numColumns;
  ResultColumn columns[MAX_NUM_COLUMNS];  // Array containing name of each column and SQL type associated with it
  uint8_t terminator;
};


struct ResultPacket {
  ResultStatus status;
  uint32_t resultSize;  // Size in bytes that he data array will be
  uint64_t* data;       // Length of the row in columns, number of columns in the row
  uint8_t terminator;
};


#endif //OPENMEMDB_LIBOMDB_H
