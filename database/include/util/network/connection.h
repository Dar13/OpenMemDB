/**
 *
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
