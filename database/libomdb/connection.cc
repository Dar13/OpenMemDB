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

#include <string>
#include <fstream>
#include <iostream>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>

#include "omdb_lib.h"
#include "serialization_helper.h"

#define MAXDATASIZE 100

using namespace libomdb;

/************************************************************************
 * Temp struct holder for results
 ************************************************************************/
/** Used to hold result before parsing into objects **/
struct ResultHolder {
  char metaDataPacket[sizeof(ResultMetaDataPacket)];
  char *resultPacket;
};


/*************************************************************************
 * Private Helper Functions                                              *
 *************************************************************************/
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/**
 * Builds a network packet with the passed in parameters.
 * This network packet is necessary to send data across sockets.
 * @param type The type of command being passed.
 * @param command The command being sent to the server
 * @return a CommandPacket containing the information to be sent across the socket.
 */
CommandPacket buildPacket(CommandType type, std::string command) {
  CommandPacket commandPacket;
  // command needs to be made into char[]
  strncpy(commandPacket.message, command.c_str(), command.length());
  commandPacket.message[command.length()] = '\0';
  commandPacket.commandType = type;
  commandPacket.type = PacketType::COMMAND;
  commandPacket.terminator = THE_TERMINATOR;

  return commandPacket;
}


/**
 * Parses result packet and builds a vector of ResultRow s
 * @param packet The result packet to parse
 * @return a vector<ResultRow> containing the parsed data
 */
std::vector<libomdb::ResultRow> parseData(ResultPacket packet) {
  std::vector<libomdb::ResultRow> rows;
  // Each of the columns is 64bits, packet.resultSize is number of bytes
  // 8 bits to a byte, so 8 bytes per column, therefore number of columns
  // is packet.resultSize / 8
  uint32_t rowSizeInBytes = packet.rowLen * 8;
  uint32_t numberOfRows = (packet.resultSize / rowSizeInBytes);
  uint64_t* dataPointer = &packet.data[0];
  // data is char* so data[0] is 1 byte
  // so dataPointer++ moves up 8 bytes, to the next uint64_t
  for (uint i = 0; i < numberOfRows; ++i) {
    libomdb::ResultRow row;
    for (uint j = 0; j < packet.rowLen; ++j) {
        row.push_back(*dataPointer);
        dataPointer++; // Move pointer to next uint64_t
    }
    rows.push_back(row);
  }

  // Place empty row in ros so that first next call moves to actual data
  // This is standar op for result sets
  libomdb::ResultRow initRow;
  rows.push_back(initRow);

  return rows;
}


/**
 * Parses a ResultMetaDataPacket and builds a list of MetaDataColumns which is
 * the core of the ResultMetaData class.
 * @param packet
 * @return A vector<MetaDataColumn> describing a result set
 */
std::vector<libomdb::MetaDataColumn> parseMetaData(ResultMetaDataPacket packet) {
  std::vector<libomdb::MetaDataColumn> metaDataColumns;
  for (uint i = 0; i < packet.numColumns; ++i) {
    libomdb::MetaDataColumn column;
    column.label = std::string(packet.columns[i].name);
    column.sqlType = packet.columns[i].type;
    metaDataColumns.push_back(column);
  }
  return metaDataColumns;
}


/**
 * Parses a ResultHolder and builds a CommandResult from the contents
 * @param result The ResultHolder to parse
 * @return A CommandResult with parsed data
 */
libomdb::CommandResult parseCommandResult(ResultHolder result) {
  //ResultMetaDataPacket metaDataPacket = DeserializeResultMetaDataPacket(result.metaDataPacket);
  ResultPacket packet = DeserializeResultPacket(result.resultPacket);
  libomdb::CommandResult commandResult;
  commandResult.status = packet.status;
  commandResult.numAffected = packet.resultSize; // TODO: Confirm with neil
  return commandResult;
}


/**
 * TODO: Add error checking
 */
libomdb::Result parseQueryResult(ResultHolder holder) {
  ResultMetaDataPacket resultMetaDataPacket =
      DeserializeResultMetaDataPacket(holder.metaDataPacket);

  ResultPacket resultPacket = DeserializeResultPacket(holder.resultPacket);

  // Build result object;
  std::vector<libomdb::ResultRow> rows = parseData(resultPacket);
  std::vector<libomdb::MetaDataColumn> metaDataColumns =
      parseMetaData(resultMetaDataPacket);
  libomdb::ResultMetaData metaData =
      libomdb::ResultMetaData::buildResultMetaDataObject(metaDataColumns);
  auto result = libomdb::Result::buildResultObject(rows, metaData);

  result.status = resultMetaDataPacket.status;

  return result;
}


/**
 * Sends message across the socket passed in
 * @param message The message to send to the server
 * @param socket The file descriptor of the listening socket
 */
ResultHolder sendMessage(CommandPacket packet, int socket) {
  //printf("Packet size is: %lu\n", sizeof(packet));
  char *serializedPacket = SerializeCommandPacket(packet);
  int bytes_sent = send(socket, serializedPacket, sizeof(packet), 0);
  if (bytes_sent == -1) {
    perror("send");
    ResultHolder emptyHolder;
    return emptyHolder;
  }

  // Wait for receipt of message;
  /*
   * Results will now come in this order:
   * 1.) ResultMetaDataPacket
   * 2.) ResultPacket
   *
   * Build a struct containing both and return it.
   */
  ResultHolder holder;

  int bytes_recieved = recv(socket, holder.metaDataPacket, sizeof(holder.metaDataPacket), 0);
  if (bytes_recieved == -1) {
    perror("recv");
    ResultHolder emptyHolder;
    return emptyHolder;
  }

  ResultMetaDataPacket resultMetaDataPacket = DeserializeResultMetaDataPacket(holder.metaDataPacket);
  holder.resultPacket = new char[resultMetaDataPacket.resultPacketSize];
  // Now receive result packet
  bytes_recieved = recv(socket, holder.resultPacket, resultMetaDataPacket.resultPacketSize, 0);
  if (bytes_recieved == -1) {
    perror("recv");
    ResultHolder emptyHolder;
    return emptyHolder;
  }

  return holder;
}

/*************************************************************************
 * ConnectionMetaData Implementations                                    *
 *************************************************************************/
libomdb::ConnectionMetaData::ConnectionMetaData(std::string dbName, bool isValid)
        :m_databaseName(dbName), m_isValid(isValid) {}

std::string libomdb::ConnectionMetaData::getDbName() {
  return this->m_databaseName;
}


bool libomdb::ConnectionMetaData::isValid() {
  return this->m_isValid;
}



/*************************************************************************
 * Connection Implementations                                            *
 *************************************************************************/
libomdb::Connection libomdb::Connection::buildConnectionObj(uint16_t socket, char* buffer) {
  ConnectionPacket packet = DeserializeConnectionPacket(buffer);
  libomdb::ConnectionMetaData* connectionMetaData = new libomdb::ConnectionMetaData(packet.name, true);
  return  *new libomdb::Connection(socket, *connectionMetaData);

}


  // TODO: Break up this monstrosity. 
libomdb::Connection libomdb::Connection::connect(std::string hostname, 
                                                 uint16_t port, 
                                                 std::string db) {
  int sockfd, numbytes;
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  // Convert hostname to c string
  const char* connection_ip = hostname.c_str();

  //Convert port to c string.
  const char* port_string = std::to_string(port).c_str();

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(connection_ip, port_string,
                        &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %d\n", rv);
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return errorConnection();
  }

  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
                                p->ai_protocol)) == -1) {
        perror("client: socket");
        continue;
      }

    if (::connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("client: connect");
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return errorConnection();
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
                                                          s, sizeof s);
  freeaddrinfo(servinfo); // all done with this structure

  if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    perror("recv");
    return errorConnection();
  }

  buf[numbytes] = '\0';


  return libomdb::Connection::buildConnectionObj(sockfd, buf);
}



void libomdb::Connection::disconnect() {
  close(this->m_socket_fd);
}



libomdb::CommandResult libomdb::Connection::executeCommand(std::string command) {
  CommandPacket packet = buildPacket(CommandType::DB_COMMAND, command);
  return parseCommandResult(sendMessage(packet, this->m_socket_fd));
}



libomdb::Result libomdb::Connection::executeQuery(std::string query) {
  CommandPacket packet = buildPacket(CommandType::SQL_STATEMENT, query);
  return parseQueryResult(sendMessage(packet, this->m_socket_fd));
}



libomdb::ConnectionMetaData libomdb::Connection::getMetaData() {
  return this->m_metaData;
}

void libomdb::Connection::setMetaData(libomdb::ConnectionMetaData data) {
  this->m_metaData = data;
}


libomdb::Connection::Connection(uint64_t socket_fd, ConnectionMetaData metaData)
        :m_metaData(metaData), m_socket_fd(socket_fd) {}
