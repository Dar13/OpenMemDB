/* Copyright (c) 2016 Neil Moore, Jason Stavrinaky, Micheal McGee, Robert Medina
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies 
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef UTIL_NET_COMMON_H
#define UTIL_NET_COMMON_H

// Standard Library includes
#include <cstring>
#include <cstdint>

// Linux-specific includes
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#define CONN_BACKLOG 20

namespace omdb
{
    /**
     *  @brief Enumeration of error classifications
     *
     *  TODO: This should be split into two enums, one for broad categories and
     *        one for specific errors within those categories
     */
    enum NetworkStatusCodes
    {
        D_RECV_PART = 4,
        D_SEND_PART = 3,
        D_RECV_FULL = 2,
        D_SEND_FULL = 1,
        SUCCESS = 0,
        E_INV_PARAM = -1,
        E_ADDRINFO = -2,
        E_SOCKET = -3,
        E_BIND = -4,
        E_LISTEN = -5,
        E_ACCEPT = -6,
        E_SEND = -7,
        E_RECV = -8,
        E_CLOSE = -9
    };

    /**
     *  @brief Struct to wrap both project-specific errors and platform errors.
     *
     *  @detail The status code is the overarching code that determines whether
     *          the secondary code should be considered a platform error or a
     *          project-specific error.
     */
    struct NetworkStatus
    {
        NetworkStatus(uint32_t code = NetworkStatusCodes::SUCCESS,
                      int32_t sec_code = 0)
            : status_code(code), secondary_code(sec_code)
        {}

        /**
         *  The status code used to classify errors.
         */
        int32_t status_code;

        /** 
         *  The platform error code or a subclassification
         *  of the project-specific status code.
         */
        int32_t secondary_code;     
    };

    NetworkStatus ListenToPort(uint16_t port_id, 
                               uint32_t* socket_fd, 
                               bool dont_block = true);

    NetworkStatus AcceptConnection(uint32_t socket_fd, 
                                   uint32_t* conn_fd,
                                   sockaddr_storage* conn_addr);
}

#endif
