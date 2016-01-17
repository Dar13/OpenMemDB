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


#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <iostream>


#include "connection.h"

#define MAXDATASIZE 100

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
  strncpy(commandPacket.message, command.c_str(), sizeof(commandPacket.message));
  commandPacket.message[sizeof(commandPacket.message) -1] = 0;
  commandPacket.commandType = type;
  commandPacket.terminator = THE_TERMINATOR;

  return commandPacket;
}

libomdb::Connection buildConnectionObj(int socket, char* buffer) {
  // TODO: Parse connection string and create new Connection object

}

libomdb::CommandResult parseCommandResult(std::string result) {
  //TODO: build CommandResult requires parsing neils string
}

libomdb::Result parseQueryResult(std::string result) {
  //TODO: build Result, requires parsing Neils stirng
  // The result packet comes in two stages, first the ResultMetaDataPacket. Then the ResultPacket.
}

/**
 * Sends message across the socket passed in
 * @param message The message to send to the server
 * @param socket The file descriptor of the listening socket
 */
std::string sendMessage(CommandPacket packet, int socket) {
  // Need to convert message to c string in order to send it.
  char message[MESSAGE_SIZE];
  memcpy(message, &packet, sizeof(packet));
  std::cout << "Message being sent: " << message <<std::endl;
  // const char* c_message = message.c_str();
  std::cout << "Attempting to send message to socket" << socket << std::endl;
  int bytes_sent = send(socket, message, sizeof message, 0);
  std::cout<< "Bytes sent: " << bytes_sent << std::endl;
  if (bytes_sent == -1) {
    perror("send");
    return NULL;
  }

  // Wait for receipt of message;
  char result[MAXDATASIZE];
  int bytes_recieved = recv(socket, result, sizeof(result), 0);
  std::cout << "Bytes relieved: " << bytes_recieved << std::endl;
  if (bytes_recieved == -1) {
    perror("recv");
    return NULL;
  }

  return result;
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
  printf("client: connecting to %s\n", s);
  freeaddrinfo(servinfo); // all done with this structure

  if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    perror("recv");
    return errorConnection();
  }

  buf[numbytes] = '\0';

  printf("client: received '%s'\n",buf);
  std::string dbConnectString = "k:"+db;
  const char* dbString = dbConnectString.c_str();
  // Sending the name of the database to the server.
  int bytesSent = send(sockfd, dbString, dbConnectString.length(), 0);
  if (bytesSent == -1) {
    perror("send");
    return errorConnection();
  }

  int bytesReceived = recv(sockfd, buf, MAXDATASIZE -1, 0);
  if (bytesReceived == -1) {
    perror("recv");
    return errorConnection();
  }

  buf[bytesReceived] = '\0';
  printf("client received response from server: %s\n", buf);

  return buildConnectionObj(sockfd, buf);
}



void libomdb::Connection::disconnect() {
  close(this->m_socket_fd);
}



libomdb::CommandResult libomdb::Connection::executeCommand(std::string command) {
  CommandPacket packet = buildPacket(CommandType::DB_COMMAND, command);
  return parseCommandResult(sendMessage(packet, this->m_socket_fd));
  // TODO: Change command to packet when sendMessage is fixed
}



libomdb::Result libomdb::Connection::executeQuery(std::string query) {
  CommandPacket packet = buildPacket(CommandType::SQL_STATEMENT, query);
  return parseQueryResult(sendMessage(packet, this->m_socket_fd));
  // TODO: Change query to packet when sendMessage is fixed
}



libomdb::ConnectionMetaData libomdb::Connection::getMetaData() {
  return this->m_metaData;
}

void libomdb::Connection::setMetaData(libomdb::ConnectionMetaData data) {
  this->m_metaData = data;
}


libomdb::Connection::Connection(uint64_t socket_fd, ConnectionMetaData metaData)
        :m_metaData(metaData), m_socket_fd(socket_fd) {}