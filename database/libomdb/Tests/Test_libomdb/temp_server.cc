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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "../../../include/util/libomdb.h"


/*
 * temp_server.cc
 *
 *  Created on: Oct 19, 2015
 *  @author Mike McGee
 */
#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold

/********
 * Stock ResultPacket and ResultMetaDataPacket to be sent back in tests
 * These will need to be serialized before being sent back to client
 */
ResultPacket getStockResultPacket() {
    uint8_t rows = 10, columns = 10;
    uint64_t* data = new uint64_t[(rows*columns)];
    for (uint64_t i = 0; i < rows*columns; ++i) {
        data[i] = i;
    }
    ResultPacket resultPacket;
    resultPacket.type = PacketType::RESULT_DATA;
    resultPacket.status = ResultStatus::OK;
    resultPacket.rowLen = rows;
    resultPacket.resultSize = (rows * columns * sizeof(uint64_t));
    resultPacket.data = data;
    resultPacket.terminator = THE_TERMINATOR;
    return resultPacket;
}

ResultMetaDataPacket getStockResultMetaDataPacket() {
    ResultColumn columns[10];
    for (int i = 0; i < 10; ++i) {
        ResultColumn col;
        std::string num(i);
        col.name = "Name "+num;
        col.type = 1; // TODO: Change to Neils types
        columns[i] = col;
    }
    ResultMetaDataPacket resultMetaDataPacket;
    resultMetaDataPacket.type = PacketType::RESULT_METADATA;
    resultMetaDataPacket.status = ResultStatus::OK;
    resultMetaDataPacket.numColumns = 10;
    resultMetaDataPacket.columns = mdColumns;
    resultMetaDataPacket.terminator = THE_TERMINATOR;
    return resultMetaDataPacket;
}

ResultPacket getStockComResult() {
    /*for command
    *  resultSize // number of rows affected
    *  status // status
    *  rowLen // errorCode
    */
    ResultPacket packet;
    packet.resultSize = 10; // 10 rows affected
    packet.status = ResultStatus::OK;
    packet.rowLen = 1; // ManipStatus is a uint32_t, don't know how this would work
    packet.data = nullptr;
    packet.terminator = THE_TERMINATOR;
    return packet;
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            if (send(new_fd, "Hello, world!", 13, 0) == -1) {
                perror("send");
                exit(1);
            }

            while(1) {
                char receivedBuffer[1000];
                int bytesReceived = recv(new_fd, receivedBuffer, 999, 0);
                if (bytesReceived == -1) {
                    perror("recv");
                    exit(1);
                }
                receivedBuffer[bytesReceived] = '\0';
                std::string messageToReturn;
                std::string mdToReturn;
                // Check if message is select or command
                if (receivedBuffer[0] == 'S' || receivedBuffer[0] == 's') {
                    messageToReturn = SerializeResultPacket(getStockResultPacket());
                    mdToReturn = SerializeResultMetaDataPacket(getStockResultMetaDataPacket());
                } else {
                    // Command results should be returned in
                    // ResultPacket with different values set
                    // see libomdb.h for specifics
                    messageToReturn = SerializeResultPacket(getStockComRes());
                    mdToReturn = SerializeResultMetaData(getStockResultMetaDataPacket());
                }
                printf("Server received message %s\n", receivedBuffer);
                if (receivedBuffer[0] == '1') {
                    break;
                }
                int bytesSent = send(new_fd, messageToReturn.c_str(), messageToReturn.size(), 0);
                if (bytesSent == -1) {
                    perror("send");
                    exit(1);
                }

                int bytesSent2 = send(new_fd, mdToReturn.c_str(), mdToReturn.size(), 0);
                if (bytesSent2 == -1) {
                    perror("send");
                    exit(1);
                }
            }
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this

    }

    return 0; // Unreachable, but that's fine I guess.
}