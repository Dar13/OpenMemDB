/**
 *
 */

#ifndef UTIL_NET_CONN_H
#define UTIL_NET_CONN_H

// Project includes
#include <util/network/common.h>

// Linux-specific includes
#include <sys/types.h>

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

        bool IsValid();

        NetworkStatus send(const char* in_buffer, size_t length);
        
        NetworkStatus recv(char* out_buffer, size_t length);

    private:
        bool m_is_valid;
        uint32_t m_socket_fd;
        sockaddr_storage m_conn_info;
    };
}

#endif
