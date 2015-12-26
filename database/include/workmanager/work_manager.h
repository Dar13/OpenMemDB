/**
 *
 */

#ifndef WORKMGR_H
#define WORKMGR_H

// Tervel includes
#include <tervel/util/tervel.h>

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

    // NM: TODO:Not sure about these error codes...
    /**
     *  @brief An enumeration of all error codes that can be returned when
     *         running any member function.
     */
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

    bool ReceiveCommand(omdb::Connection& conn);

    //! Tervel object to give to the worker threads
    tervel::Tervel* m_tervel;

    // TODO: Figure out scalable replacement
    std::array<WorkThreadData, 8> m_thread_data;

    //! Holds the futures for currently running jobs
    std::vector<std::future<JobResult>> m_thread_results;

    //! The listening server socket file descriptor
    uint32_t m_server_socket_fd;

    //! The list of current connections
    std::vector<omdb::Connection> m_connections;

    //! The mapping of job to connection
    std::map<uint32_t, omdb::Connection> m_job_to_connection;
};

#endif
