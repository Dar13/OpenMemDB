/**
 *
 */

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
        Connection(int socket_fd);

        //void send(std::string& msg);
        
        //std::string recv();
    private:
        uint32_t socket_fd;
    };
}
