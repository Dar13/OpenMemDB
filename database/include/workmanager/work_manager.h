/* Copyright (c) 2016 Neil Moore, Jason Stavrinaky, Micheal McGee, Robert Medina
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies 
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
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

class DataStore;

/**
 *  @brief Class that handles the main thread and load balancing.
 */
class WorkManager
{
public:
    WorkManager(uint32_t num_threads, tervel::Tervel* tervel);
    ~WorkManager()
    {
        delete m_context;
    }

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

    //! This constant also determines the maximum number of threads
    //! in the system.
    static const uint32_t MAX_NUM_MUTEXES = 64;

private:

    uint32_t GetAvailableThread();

    bool ReceiveCommand(omdb::Connection& conn);

    bool SendResult(omdb::Connection& conn, ResultBase* result);

    //! Tervel object to give to the worker threads
    tervel::Tervel* m_tervel;

    //! Tervel thread context so the work manager can initialize Tervel objects
    tervel::ThreadContext* m_context;

    //! Number of threads that are supposed to be running
    uint32_t m_num_threads;

    //! Worker thread data storage
    std::vector<WorkThreadData> m_thread_data;

    //! Holds the futures for currently running jobs
    std::vector<std::future<JobResult>> m_thread_results;

    //! The listening server socket file descriptor
    uint32_t m_server_socket_fd;

    //! The list of current connections
    std::vector<omdb::Connection> m_connections;

    //! Mutex pool
    ThreadNotifier m_thread_notifiers[MAX_NUM_MUTEXES];

    //! The mapping of job to connection
    std::map<uint32_t, omdb::Connection> m_job_to_connection;

    //! The active data store object
    DataStore* data_store;
};

#endif
