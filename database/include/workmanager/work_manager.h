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

// Linux-specific includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// Project includes
#include "types.h"

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
        E_BIND_ERR      = -2,
        E_LISTEN_ERR    = -3,
        E_OTHER_ERR     = -4
    };

    /**
     *  @brief Initialize the work manager, set up data structures and prepare to do work.
     *
     *  @return Any return value less than 0 is an error code(@refer WorkManager::Errors)
     */
    int32_t Initialize();

    int32_t Run();

private:
    tervel::Tervel* m_tervel;

    std::array<WorkThreadData, 8> m_thread_data;
    std::vector<std::future<Result>> m_thread_results;

};

#endif
