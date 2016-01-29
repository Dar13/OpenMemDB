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

// Standard Library includes
#include <cerrno>
#include <string>

// Linux-specific includes
#include <unistd.h>

// Project includes
#include <util/network/common.h>

/**
 *  @brief Creates and initializes a socket that listens to the given port.
 *
 *  @note TODO: Replace raw socket file descriptor with @refer omdb::Connection object
 */
omdb::NetworkStatus omdb::ListenToPort(uint16_t port_id,
                                       uint32_t* socket_fd,
                                       bool dont_block)
{
    // If the pointer isn't valid, can't do anything with it.
    if(socket_fd == nullptr)
    {
        return NetworkStatus(omdb::E_INV_PARAM);
    }

    addrinfo hints;
    addrinfo* name_info_list;

    int32_t status = 0;

    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;         // Use either IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;     // TCP
    hints.ai_flags = AI_PASSIVE;         // Server address

    std::string port_id_str = std::to_string(port_id);

    // Gets all possible address/port pairs we can bind to.
    status = getaddrinfo(NULL, port_id_str.c_str(),
                         &hints, &name_info_list);

    if(status != 0)
    {
        return NetworkStatus(omdb::E_ADDRINFO, status);
    }

    int sock_flags = 0;
    if(dont_block)
    {
        sock_flags = SOCK_NONBLOCK;
    }

    addrinfo* final_addr = nullptr;
    for(addrinfo* itr = name_info_list; itr != nullptr; itr = itr->ai_next)
    {
        status = socket(itr->ai_family, 
                        itr->ai_socktype | sock_flags,
                        itr->ai_protocol);

        if(status == -1)
        {
            continue;
        }
        else
        {
            *socket_fd = status;
        }

        int32_t opt_status = 0;
        status = setsockopt(*socket_fd, SOL_SOCKET, SO_REUSEADDR,
                            &opt_status, sizeof(int32_t));

        if(status == -1)
        {
            return omdb::NetworkStatus(omdb::E_SOCKET, errno);
        }
        
        status = bind(*socket_fd, itr->ai_addr, itr->ai_addrlen);
        if(status == -1)
        {
            close(*socket_fd);
            continue;
        }

        final_addr = itr;
        break;
    }

    freeaddrinfo(name_info_list);

    if(final_addr == nullptr)
    {
        // NOTE: This only returns the last error!
        return omdb::NetworkStatus(omdb::E_BIND, errno);
    }

    // Attempt to listen to the bound socket/port pair.
    status = listen(*socket_fd, CONN_BACKLOG);
    if(status == -1)
    {
        return omdb::NetworkStatus(omdb::E_LISTEN, errno);
    }

    return omdb::NetworkStatus(omdb::SUCCESS);
}

/**
 *  @brief Attempt to accept a connection waiting on the socket's assigned port.
 *
 *  @note TODO: Replace the raw socket file descriptor with omdb::Connection
 */
omdb::NetworkStatus omdb::AcceptConnection(uint32_t socket_fd,
                                           uint32_t* conn_fd,
                                           sockaddr_storage* conn_addr)
{
    socklen_t addr_len = 0;

    // Needed to use a special version of accept() to make sure it's non-blocking
    int32_t status = accept4(socket_fd, reinterpret_cast<sockaddr*>(conn_addr), 
                            &addr_len, SOCK_NONBLOCK);

    // Error handling
    if(status == -1)
    {
        *conn_fd = 0;

        if(errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return omdb::NetworkStatus(omdb::SUCCESS);
        }

        return omdb::NetworkStatus(omdb::E_ACCEPT, errno);
    }

    *conn_fd = status;

    return omdb::NetworkStatus(omdb::SUCCESS);
}
