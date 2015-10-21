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

    // Set up the thread data
    uint32_t thread_id = 0;
    for(auto& thread_data : m_thread_data)
    {
        thread_data.id = thread_id;
        thread_id++;

        thread_data.stop = false;

        thread_data.thread = std::thread(WorkThread::Run, &thread_data);
    }

    // Create a socket and listen to it.
    omdb::NetworkStatus status = omdb::ListenToPort(0xDDD, &m_server_socket_fd, true);
    if(status.status_code != omdb::SUCCESS)
    {
        if(status.status_code == omdb::E_ADDRINFO)
        {
            printf("getaddrinfo failed!\n");
            printf("Error string: %s\n", gai_strerror(status.secondary_code));
        }
        else
        {
            printf("Error when listening to port!\n");
            printf("Code: %d Secondary: %s\n", status.status_code,
                                               strerror(status.secondary_code));
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

    omdb::NetworkStatus status;

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

    // This is a wrapper for the member function. Makes it a bit cleaner to use with
    // std::algorithm stuff.
    auto receive_command = [this] (omdb::Connection& conn) -> bool
    {
        return this->ReceiveCommand(conn);
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
            printf("Error accepting connection\n");
            printf("Code: %d Secondary Code: %d\n", status.status_code,
                                                    status.secondary_code);

            running = false;
            continue;
        }

        // Check if new connection has been made
        if(status.status_code == omdb::SUCCESS && conn_socket_fd != 0)
        {
            printf("Creating new connection!\n");
            omdb::Connection conn(conn_socket_fd, conn_addr);
            m_connections.push_back(conn);
        }

        m_connections.erase( std::remove_if( m_connections.begin(),
                                             m_connections.end(),
                                             receive_command),
                             m_connections.end());

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
                printf("Warning! Result exists without a valid connection\n");
                continue;
            }

            printf("Job %d has result %lu\n", res.job_number, res.result);

            status = conn.send((char*)&res.result, sizeof(uint64_t));
            while(status.status_code == omdb::D_RECV_PART)
            {
                printf("Attempting to send entirety of response\n");
                status = conn.send((char*)res.result, sizeof(uint64_t));
            }

            if(status.status_code != omdb::D_SEND_FULL &&
               status.status_code != omdb::SUCCESS)
            {
                printf("Error sending result to client!\n");
                printf("Code: %d Secondary: %s\n", status.status_code,
                                                   strerror(status.secondary_code));
                return E_NET_ERR;
            }

            printf("Job number %d is done!\n", res.job_number);
        }
        results.clear();
    }

    std::cout << "All done! Exiting..." << std::endl;

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

/**
 *  @brief Handle incoming commands from client connections.
 */
bool WorkManager::ReceiveCommand(omdb::Connection& conn)
{
    // TODO: Make magic number into a const
    char buffer[256];
    
    // We're making this unsigned so the overflow behavior is predictable
    // If we ever get more than 3 billion concurrent requests, we'll be in trouble though...
    static uint32_t job_number = 0;

    omdb::NetworkStatus status;

    // Is there anything in the queue?
    // TODO: Make magic number into a const
    status = conn.recv(buffer, 256);

    // Generate a job and queue it into a worker thread if the full 
    // message was received
    if(status.status_code == omdb::D_RECV_FULL)
    {
        job_number++;

        // TODO: Remove this, for debugging purposes only
        printf("Creating job number %d\n", job_number);

        // Create the worker thread's task
        Job j = WorkThread::GenerateJob(job_number, buffer);
        m_thread_results.push_back(j.get_future());
        m_job_to_connection[job_number] = conn;

        // Push the job to the thread
        // TODO: Replace the lock and queue push with a Tervel FIFO
        WorkThreadData& thread_data = m_thread_data[GetAvailableThread()];

        std::unique_lock<std::mutex> lock(thread_data.mutex);
        thread_data.jobs.push(std::move(j));

        // Notify the thread to wake-up
        thread_data.cond_var.notify_one();

        status.status_code = omdb::SUCCESS;
        return false;
    }

    if(status.status_code == omdb::E_CLOSE)
    {
        printf("Connection closed!\n");
        conn.close();
        return true;
    }

    if(status.status_code == omdb::E_RECV)
    {
        printf("Error receiving command\n");
        printf("Code: %d Secondary: %s\n", status.status_code,
                                           strerror(status.secondary_code));

        return true;
    }

    return false;
}
