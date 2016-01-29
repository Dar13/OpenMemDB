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

/*
 * connector.cc
 *
 *  Created on: Oct 17, 2015
 *  @author Mike McGee
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
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <fstream>


#include "connector.h"
#include "resultset.h"

#define MAXDATASIZE 100 // max number of bytes we can get at once

/**
 * get sockaddr, IPv4 or IPv6:
 * @param sa
 *
 * @note private helper function for connection.
 */
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/**
 * Connects to the specified port and database.
 * @param port_number The port to connect to.
 * @param ip The host-name to connect to.
 * @return The socket file descriptor that is created in the connection
 */
uint32_t connector::Connect(uint16_t port_number, std::string ip) {
  int sockfd, numbytes;
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  // Convert ip to c string
  const char* connection_ip = ip.c_str();

  //Convert port_number to c string.
  const char* port = std::to_string(port_number).c_str();

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(connection_ip, port,
                        &hints, &servinfo)) != 0) 
  {
    fprintf(stderr, "getaddrinfo: %d\n", rv);
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      return 1;
  }

  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype,
              p->ai_protocol)) == -1) {
          perror("client: socket");
          continue;
      }
      if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
          close(sockfd);
          perror("client: connect");
          continue;
      }

      break;
  }

  if (p == NULL) {
      fprintf(stderr, "client: failed to connect\n");
      return 2;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
          s, sizeof s);
  printf("client: connecting to %s\n", s);
  freeaddrinfo(servinfo); // all done with this structure

  if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
      perror("recv");
      exit(1);
  }

  buf[numbytes] = '\0';

  printf("client: received '%s'\n",buf);

  int bytesSent = send(sockfd, "Can you hear me?", 17, 0);
  if (bytesSent == -1) {
    perror("send");
    exit(1);
  }

  int bytesReceived = recv(sockfd, buf, MAXDATASIZE -1, 0);
  if (bytesReceived == -1) {
    perror("recv");
    exit(1);
  }

  buf[bytesReceived] = '\0';
  printf("client received response from server: %s\n", buf);

  return sockfd;
}



/**
 * Sends a message to the server
 *
 * @note Every sent message expects a reply
 *
 * @param message The message to send to the server
 * @param socket_fd The socket to send the message to.
 * @return A pointer to the result set created by the command.
 */
libmem::ResultSet* connector::SendMessage(std::string message,
                                             uint32_t socket_fd) {
  // Need to convert message to c string in order to send it.
  const char* c_message = message.c_str();
  std::cout << "Attempting to send message to socket" << socket_fd << std::endl;
  int bytes_sent = send(socket_fd, c_message, sizeof c_message, 0);
  std::cout<< "Bytes sent: " << bytes_sent << std::endl;
  if (bytes_sent == -1) {
    perror("send");
    return NULL;
  }

  // Wait for receipt of message;
  libmem::ResultSet result_set;
  std::cout << "Made result_set" << std::endl;
  libmem::ResultSet* res_ptr = &result_set;

  // Need to serialize pointer here. (or maybe the whole class)
  std::ofstream ofs("ResultSetHolder.txt");
  {
    boost::archive::text_oarchive oa(ofs);
    oa << res_ptr;
  }

  //TODO: Perfrom serialization of ResultSet object on server side.
  // Done here now for testing purposes.

  //char result[MAXDATASIZE];
  int bytes_recieved = recv(socket_fd, res_ptr, sizeof(res_ptr), 0);
  std::cout << "Bytes recieved: " << bytes_recieved << std::endl;
  if (bytes_recieved == -1) {
    perror("recv");
    return NULL;
  }

  libmem::ResultSet* desrilaized_res_ptr;
  //Deserialize the pointer
  {
    std::ifstream ifs("ResultSetHolder.txt");
    boost::archive::text_iarchive ia(ifs);
    ia >> desrilaized_res_ptr;
  }

  //return result_set;
  //return res_ptr;
  return desrilaized_res_ptr;
}



/**
 * @brief Closes the connection to the socket.
 * @param socket_fd
 */
void connector::Disconnect(uint32_t socket_fd) {
  close(socket_fd);
}
