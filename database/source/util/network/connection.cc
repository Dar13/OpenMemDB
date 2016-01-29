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

// Standard library includes
#include <cerrno>
#include <cstdio>

// Linux-specific includes
#include <unistd.h>

// Project includes
#include <util/network/connection.h>

/**
 *  @brief Constructor that takes in an already-created socket and relevant 
 *         information.
 */
omdb::Connection::Connection(int32_t socket_fd, sockaddr_storage conn_info)
    : m_is_valid(true), m_socket_fd(socket_fd), m_conn_info(conn_info)
{}

/**
 *  @brief Default constructor to shut up some standard library containers.
 *
 *  @detail Sets the valid flag to false to represent that it's not actually a
 *          connection, just a placeholder.
 */
omdb::Connection::Connection()
    : m_is_valid(false)
{}

/**
 *  @brief Destructor
 */
omdb::Connection::~Connection()
{
}

/**
 *  @brief Returns if the connection has been deemed to represent an actual connection.
 */
bool omdb::Connection::IsValid()
{
    return m_is_valid;
}

/**
 *  @brief Closes the associated socket file descriptor
 */
void omdb::Connection::close()
{
    ::close(m_socket_fd);
}

/**
 *  @brief Sends the content in the given buffer over the associated socket.
 */
omdb::NetworkStatus omdb::Connection::send(const char* in_buffer, size_t length)
{
    // TODO: Let the caller know this is an invalid connection!
    if(!m_is_valid) 
    { 
        return omdb::NetworkStatus(omdb::E_SEND, 0);
    }

    int32_t status = ::send(m_socket_fd, reinterpret_cast<const void*>(in_buffer),
                            length, MSG_DONTWAIT);

    // Error handling
    if(status == -1)
    {
        // Have to check for both for portability reasons
        if(errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return omdb::NetworkStatus(omdb::D_SEND_PART);
        }

        // Some error has occurred
        return omdb::NetworkStatus(omdb::E_SEND, errno);
    }
    else
    {
        return omdb::NetworkStatus(omdb::D_SEND_FULL);
    }
}

/**
 *  @brief Fills the given buffer with whatever data is queued up for this socket.
 */
omdb::NetworkStatus omdb::Connection::recv(char* out_buffer, size_t length)
{
    // TODO: Let the caller know this is an invalid connection!
    if(!m_is_valid)
    {
        return omdb::NetworkStatus(omdb::E_RECV);
    }

    int32_t status = ::recv(m_socket_fd, reinterpret_cast<void*>(out_buffer),
                            length, MSG_DONTWAIT);

    // Check if the TCP connection has closed
    if(status == 0)
    {
        return omdb::NetworkStatus(omdb::E_CLOSE);
    }

    if(status == -1)
    {
        // Check for the need to call recv() again
        if(errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return omdb::NetworkStatus(omdb::D_RECV_PART);
        }

        // Another check if the connection is closed
        if(errno == ECONNRESET)
        {
            return omdb::NetworkStatus(omdb::E_CLOSE);
        }

        // Some other error has occurred
        return omdb::NetworkStatus(omdb::E_RECV, errno);
    }
    else
    {
        return omdb::NetworkStatus(omdb::D_RECV_FULL);
    }
}
