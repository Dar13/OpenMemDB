/**
 *
 */

// Standard library includes
#include <cerrno>

// Project includes
#include <util/network/connection.h>

omdb::Connection::Connection(int32_t socket_fd, sockaddr_storage conn_info)
    : m_is_valid(true), m_socket_fd(socket_fd), m_conn_info(conn_info)
{}

omdb::Connection::Connection()
    : m_is_valid(false)
{}

bool omdb::Connection::IsValid()
{
    return m_is_valid;
}

omdb::NetworkStatus omdb::Connection::send(const char* in_buffer, size_t length)
{
    int32_t status = ::send(m_socket_fd, reinterpret_cast<const void*>(in_buffer),
                          length, MSG_DONTWAIT);
    if(status == -1)
    {
        if(errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return omdb::NetworkStatus(omdb::D_SEND_PART);
        }

        return omdb::NetworkStatus(omdb::E_SEND, errno);
    }
    else
    {
        return omdb::NetworkStatus(omdb::D_SEND_FULL);
    }
}

omdb::NetworkStatus omdb::Connection::recv(char* out_buffer, size_t length)
{
    int32_t status = ::recv(m_socket_fd, reinterpret_cast<void*>(out_buffer),
                            length, MSG_DONTWAIT);

    if(status == -1)
    {
        if(errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return omdb::NetworkStatus(omdb::D_RECV_PART);
        }

        return omdb::NetworkStatus(omdb::E_RECV, errno);
    }
    else
    {
        return omdb::NetworkStatus(omdb::D_RECV_FULL);
    }
}
