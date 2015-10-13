/*
 *
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
    enum NetworkStatusCodes
    {
        D_RECV_PART = 4,
        D_SEND_PART = 3,
        D_RECV_FULL = 2,
        D_SEND_FULL = 1,
        SUCCESS = 0,
        E_INV_PARAM,
        E_ADDRINFO,
        E_SOCKET,
        E_BIND,
        E_LISTEN,
        E_ACCEPT,
        E_SEND,
        E_RECV,
        E_CLOSE,
        E_NUM_ERRORS
    };

    struct NetworkStatus
    {
        NetworkStatus(uint32_t code = NetworkStatusCodes::SUCCESS,
                      int32_t sec_code = 0)
            : status_code(code), secondary_code(sec_code)
        {
        }

        uint32_t status_code;
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
