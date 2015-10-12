/*
 *
 */

#ifndef UTIL_NET_COMMON_H
#define UTIL_NET_COMMON_H

#define CONN_BACKLOG 20

namespace omdb
{
    enum NetworkErrorCodes
    {
        E_NONE = 0,
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

    struct NetworkError
    {
        NetworkError(uint32_t code = NetworkErrorCodes::E_NONE,
                     int32_t sec_code = 0)
            : error_code(code), secondary_code(sec_code)
        {
        }

        uint32_t error_code;
        int32_t secondary_code;
    };

    NetworkError ListenToPort(uint16_t port_id, 
                              uint32_t* socket_fd, 
                              bool dont_block = true);

    NetworkError AcceptConnection(uint32_t socket_fd, 
                                  uint32_t* conn_fd,
                                  sockaddr* conn_addr);
}

#endif
