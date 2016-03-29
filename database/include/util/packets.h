//
// Created by mike on 1/14/16.
//

#ifndef OPENMEMDB_LIBOMDB_H
#define OPENMEMDB_LIBOMDB_H

// C++ stdlib include
#include <string>

// Project includes
#include <util/result.h>

#define MESSAGE_SIZE 300

const uint8_t THE_TERMINATOR = 0xFF;
const uint32_t DB_NAME_LEN = 50;
const uint32_t COL_NAME_LEN = 25;
const uint32_t MAX_NUM_COLUMNS = 20;

/**
 * Enum used to describe the type of statement being sent
 */
enum class CommandType: uint8_t {
  SQL_STATEMENT = 0x01,
  DB_COMMAND = 0x02,
  INVALID_COMMAND
};

/**
 * Enum used to describe the type of packet being sent
 */
enum class PacketType: uint8_t {
  NONE = 0,
  COMMAND,
  COMMAND_RESULT,
  CONNECTION,
  RESULT_METADATA,
  RESULT_DATA,
  INVALID_PACKET
};

/** Represents result of database command */
struct CommandResult {
    bool isSuccess;
    int numAffected;
};

struct CommandPacket {
    PacketType type;
  CommandType commandType;  // The type of command requested to be executed: SQL_STATEMENT or DB_COMMAND
  char message[MESSAGE_SIZE];
  uint8_t terminator;
}__attribute__((packed));

struct CommandResultPacket {
    PacketType type;
    ResultStatus status;
    ManipStatus secondaryStatus;
    uint32_t rowsAffected;
    uint8_t terminator;
} __attribute__((packed));

struct ConnectionPacket {
  PacketType type;
  /** The name of the database */
  char name[DB_NAME_LEN]; // The name of the database connected to.
}__attribute__((packed));


struct ResultColumn {
  char name[COL_NAME_LEN];  // The column name
  uint16_t type;            // The SQL type of the column
}__attribute__((packed));


struct ResultMetaDataPacket {
  PacketType type;
  ResultStatus status;
  uint32_t numColumns;
  uint32_t resultPacketSize; // Size of the ResultPacket that follows this packet
  ResultColumn columns[MAX_NUM_COLUMNS];  // Array containing name of each column and SQL type associated with it
  uint8_t terminator;
}__attribute__((packed));

/**
 * Used to return results in both queries and commands
 * for query see comments beside field
 * for command
 *  resultSize // number of rows affected
 *  status // status
 *  rowLen // errorCode
 */
struct ResultPacket {
    PacketType type;      // 1 byte
    ResultStatus status;  // 2 bytes
    uint16_t rowLen;      // Length of the row in columns, number of columns in the row
    uint32_t resultSize;  // Size in bytes that he data array will be
    uint64_t* data;
    uint8_t terminator;
} __attribute__((packed));


#endif //OPENMEMDB_LIBOMDB_H
