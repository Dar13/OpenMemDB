/*
 *
 */

#include <util/network/common.h>

omdb::NetworkError omdb::ListenToPort(uint16_t port_id,
                                      uint32_t* socket_fd,
                                      bool dont_block)
{
    if(socket_fd == nullptr)
    {
        return NetworkError(omdb::E_INV_PARAM);
    }

    addrinfo hints;
    addrinfo* name_info_list;

    int32_t status = 0;

    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;                        // Use either IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM | SOCK_NONBLOCK;    // non-blocking TCP
    hints.ai_flags = AI_PASSIVE;                        // Server address

    status = getaddrinfo(NULL, port_id, &hints, &name_info_list);
    if(status != 0)
    {
        return NetworkError(E_ADDRINFO, status);
    }

    addrinfo* final_addr = nullptr;
    for(addrinfo* itr = name_info_list; itr != nullptr; itr = itr->ai_next)
    {
        status = socket(itr->ai_family, itr->ai_socktype, itr->ai_protocol);
        if(status == -1)
        {
            continue;
            //return NetworkError(E_SOCKET, errno);
        }
        else
        {
            *socket_fd = status;
        }

        int32_t opt_status = 0;
        status = setsockopt(*socket_fd, SOL_SOCKET, SO_REUSEADDR,
                            *opt_status, sizeof(int32_t));

        if(status == -1)
        {
            return omdb::NetworkError(E_SOCKET, errno);
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
        return omdb::NetworkError(E_BIND, errno);
    }

    status = listen(*socket_fd, CONN_BACKLOG);
    if(status == -1)
    {
        return omdb::NetworkError(E_LISTEN, errno);
    }

    return omdb::NetworkError();
}


