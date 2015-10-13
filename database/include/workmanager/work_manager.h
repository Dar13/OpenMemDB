/**
 *
 */

#ifndef WORKMGR_H
#define WORKMGR_H

// Tervel includes
#include <tervel/util/tervel.h>
//#include <tervel/containers/wf/vector/vector.hpp>
//#include <tervel/containers/wf/hash-map/wf_hash_map.h>

// Standard library includes
#include <thread>
#include <map>
#include <vector>

// Linux-specific includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// Project includes
#include <workmanager/types.h>
#include <util/network/connection.h>

/**
 *  @brief Class that handles the main thread and load balancing.
 */
class WorkManager
{
public:
    WorkManager(uint32_t num_threads, tervel::Tervel* tervel);

    // NM: Not sure about these error codes...
    enum Errors
    {
        E_NONE          = 0,
        E_TERVEL_ERR    = -1,
        E_NET_ERR      = -2,
        E_OTHER_ERR     = -3
    };

    int32_t Initialize();

    int32_t Run();

private:

    uint32_t GetAvailableThread();

    tervel::Tervel* m_tervel;

    std::array<WorkThreadData, 8> m_thread_data;
    std::vector<std::future<Result>> m_thread_results;

    uint32_t m_server_socket_fd;
    std::vector<omdb::Connection> m_connections;
    std::map<uint32_t, omdb::Connection> m_job_to_connection;
};

#endif
