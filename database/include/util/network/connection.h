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

#ifndef UTIL_NET_CONN_H
#define UTIL_NET_CONN_H

// Standard Library includes
#include <cstdio>

// Linux-specific includes
#include <sys/types.h>

// Project includes
#include <util/network/common.h>

namespace omdb
{
    /**
     *  @brief This class represents a connection between this application
     *         and one of its clients.
     *
     *  @note The amount of abstraction done by this class has not been decided yet.
     */
    class Connection
    {
    public:
        Connection();
        Connection(int32_t socket_fd, sockaddr_storage conn_info);

        ~Connection();

        bool IsValid();

        void close();

        NetworkStatus send(const char* in_buffer, size_t length);
        
        NetworkStatus recv(char* out_buffer, size_t length);

    private:
        //! Holds the validity of this connection(valid socket, active connection, etc.).
        bool m_is_valid;

        //! The raw socket file descriptor
        uint32_t m_socket_fd;

        //! Holds miscellaneous information about the connected peer.
        sockaddr_storage m_conn_info;
    };
}

#endif
