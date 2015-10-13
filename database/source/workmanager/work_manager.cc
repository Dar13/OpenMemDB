/**
 *
 */

// Standard Library includes
#include <cstring>
#include <map>
#include <algorithm>

// Project includes
#include <workmanager/work_manager.h>

WorkManager::WorkManager(uint32_t num_threads, tervel::Tervel* tervel)
    : m_tervel(tervel)
{
}

/**
 *  @brief Initialize the internal data structures and members, prepare to 
 *         begin executing tasks.
 */
int32_t WorkManager::Initialize()
{
    std::cout << "Initializing WorkManager\n";

    uint32_t thread_id = 0;
    for(auto& thread_data : m_thread_data)
    {
        thread_data.id = thread_id;
        thread_id++;

        thread_data.stop = false;

        thread_data.thread = std::thread(WorkThread::Run, &thread_data);
    }

    
    omdb::NetworkStatus status = omdb::ListenToPort(0xDDD, &m_server_socket_fd, true);
    if(status.status_code != omdb::SUCCESS)
    {
        if(status.status_code == omdb::E_ADDRINFO)
        {
            std::cout << "getaddrinfo failed!\n";
            std::cout << "Error string: " << gai_strerror(status.secondary_code) << std::endl;
        }
        else
        {
            std::cout << "Error when listening to port!\n";
            std::cout << "Error code: " << status.status_code << " Secondary Code: " << strerror(status.secondary_code) << std::endl;
        }

        return E_NET_ERR;
    }

    return E_NONE;
}

/**
 *  @brief Start the processing of client requests/commands, this method is 
 *         essentially starting the database server.
 */
int32_t WorkManager::Run()
{
    std::cout << "Running WorkManager::Run()\n";

    // We're making this unsigned so the overflow behavior is predictable
    uint32_t job_number = 0;

    // Buffer to use for network operations
    // TODO: Make magic number a const
    char buffer[256];

    omdb::NetworkStatus status;

    //int job_num = 0;
    //for(auto& thread_data : m_thread_data)
    //{
    //    for(int i = 0; i < 2; i++)
    //    {
    //       Job j = WorkThread::GenerateJob(job_num, "Hello World!"); 
    //       m_thread_results.push_back(j.get_future());
    //       job_num++;

    //       std::unique_lock<std::mutex> lock(thread_data.mutex);
    //       thread_data.jobs.push(std::move(j));
    //    }

    //    thread_data.cond_var.notify_one();
    //}
    
    // Here's where we store the results
    std::vector<Result> results;

    // Quick lambda for use in std::remove_if()
    auto check_future_result = [&results] (std::future<Result>& res) -> bool
    {
        if(res.valid() &&
           res.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            results.push_back(res.get());
            return true;
        }

        return false;
    };


    std::cout << "Starting the main loop!\n";

    // The main loop
    bool running = true;
    while(running)
    {
        uint32_t conn_socket_fd = 0;

        sockaddr_storage conn_addr;
        memset(&conn_addr, 0, sizeof(sockaddr_storage));

        status = omdb::AcceptConnection(m_server_socket_fd,
                                        &conn_socket_fd,
                                        &conn_addr);

        // Check for errors
        if(status.status_code != E_NONE)
        {
            std::cout << "Error while attempting to accept connection!\n";
            std::cout << "Code: " << status.status_code << " Secondary: " << strerror(status.secondary_code) << std::endl;

            running = false;
            continue;
        }

        // Check if new connection has been made
        if(status.status_code == omdb::SUCCESS && conn_socket_fd != 0)
        {
            omdb::Connection conn(conn_socket_fd, conn_addr);
            m_connections.push_back(conn);
        }

        // Now to check existing connections for commands
        for(omdb::Connection& conn : m_connections)
        {
            // Is there anything in the queue?
            // TODO: Make magic number into a const
            status = conn.recv(buffer, 256);

            // Generate a job and queue it into a worker thread if the full 
            // message was received
            if(status.status_code == omdb::D_RECV_FULL)
            {
                job_number++;

                Job j = WorkThread::GenerateJob(job_number, buffer);
                m_thread_results.push_back(j.get_future());
                m_job_to_connection[job_number] = conn;

                WorkThreadData& thread_data = m_thread_data[GetAvailableThread()];

                std::unique_lock<std::mutex> lock(thread_data.mutex);
                thread_data.jobs.push(std::move(j));
            }
        }

        // This checks if any of the futures in this vector are finished and erases them
        // from the vector if they are.
        m_thread_results.erase( std::remove_if(m_thread_results.begin(),
                                               m_thread_results.end(),
                                               check_future_result),
                                m_thread_results.end());

        for(Result res : results)
        {
            omdb::Connection conn;
            if(m_job_to_connection.find(res.job_number) != m_job_to_connection.end())
            {
                conn = m_job_to_connection[res.job_number];
            }
            else
            {
                std::cout << "Warning! Result exists without a valid connection\n";
                continue;
            }

            status = conn.send((char*)res.result, sizeof(uint64_t));
            while(status.status_code == omdb::D_RECV_PART)
            {
                status = conn.send((char*)res.result, sizeof(uint64_t));
            }

            if(status.status_code != omdb::D_SEND_FULL ||
               status.status_code != omdb::SUCCESS)
            {
                std::cout << "Error sending result to client!\n";
                std::cout << "Code: " << status.status_code;
                std::cout << " Secondary: " << status.secondary_code << std::endl;
                return E_NET_ERR;
            }
        }
    }

    //bool waiting = true;
    //while(waiting)
    //{
    //    bool all_done = true;
    //    for(auto& result : m_thread_results)
    //    {
    //        if(result.valid() && 
    //           result.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    //        {
    //            // TODO: Return result to proper connection
    //            std::cout << "Job num " << result.get();
    //            std::cout << " is done!" << std::endl;
    //        }
    //        else
    //        {
    //            if(result.valid())
    //            {
    //                all_done = false;
    //            }
    //        }
    //    }

    //    waiting = (!all_done);
    //}

    std::cout << "All done!" << std::endl;

    return E_NONE;
}

/**
 *  @brief Performs load-balancing to spread tasks over the available threads.
 *
 *  @note TODO: THIS IS TEMPORARY
 */
uint32_t WorkManager::GetAvailableThread()
{
    static uint32_t idx = 0;

    idx = (idx + 1) % 8;
    return idx;
}
