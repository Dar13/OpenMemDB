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

// Standard Library includes
#include <cstring>
#include <map>
#include <algorithm>

// Project includes
#include <util/libomdb.h>
#include <workmanager/work_manager.h>
#include <workmanager/work_thread.h>

/**
 *  \brief Constructor that initializes internal variables properly.
 */
WorkManager::WorkManager(uint32_t num_threads, tervel::Tervel* tervel)
    : m_tervel(tervel), m_num_threads(num_threads), m_thread_data(num_threads)
{
    m_context = new tervel::ThreadContext(m_tervel);
}

/**
 *  @brief Initialize the internal data structures and members, prepare to 
 *         begin executing tasks.
 */
int32_t WorkManager::Initialize()
{
    std::cout << "Initializing WorkManager\n";

    // Setup the thread data
    for(uint32_t thread_id = 0; thread_id < m_num_threads; thread_id++)
    {
        auto found_idx = std::find_if(std::begin(m_thread_notifiers),
                std::end(m_thread_notifiers),
                [] (const ThreadNotifier& m) -> bool {
                    return !m.used;
                });

        if(found_idx == std::end(m_thread_notifiers))
        {
            printf("ERROR: Requested number of threads outstrips number of "
                    "available mutexes!\n");
            return E_OTHER_ERR;
        }

        found_idx->used = true;
        WorkThreadData data(found_idx);
        data.tervel = m_tervel;

        m_thread_data.push_back(std::move(data));
        m_thread_data.back().thread = std::thread(WorkThread::Run, &m_thread_data.back());
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
    std::vector<JobResult> results;

    // Quick lambda for use in std::remove_if()
    auto check_future_result = [&results] (std::future<JobResult>& res) -> bool
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

            // Send ConnectionPacket with some database metadata
            ConnectionPacket conn_packet;
            memset(conn_packet.name, 0, DB_NAME_LEN);
            memcpy(conn_packet.name, "OpenMemDB Database", 18);

            // TODO: This may infinitely loop
            while(conn.send(reinterpret_cast<char*>(&conn_packet), 
                        sizeof(ConnectionPacket)).status_code == omdb::D_SEND_PART)
            {}
        }

        // Remove closed connections while checking if any have commands that need to 
        // be handled.
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

        // Process the current backlog of results and send them back to the client
        for(JobResult res : results)
        {
            omdb::Connection conn;
            if(m_job_to_connection.find(res.job_number) != m_job_to_connection.end())
            {
                conn = m_job_to_connection[res.job_number];
            }
            else
            {
                printf("Warning! JobResult exists without a valid connection, ignoring\n");
                continue;
            }

            printf("Job %d has result of type %u\n", res.job_number, res.result->type);

            if(!SendResult(conn, res.result))
            {
                printf("Unable to send job!\n");
            }

            // TODO: How to handle Command results?
            // TODO: Generate ResultMetaData packet
            // TODO: Generate Result packet

            /*
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
            */

            printf("Job number %d is done!\n", res.job_number);
            
            // Clean up result
            delete res.result;
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
    char buffer[MAX_PACKET_SIZE];
    
    // We're making this unsigned so the overflow behavior is predictable
    // If we ever get more than 3 billion concurrent requests, we'll be in trouble though...
    static uint32_t job_number = 0;

    omdb::NetworkStatus status;

    // Is there anything in the queue?
    status = conn.recv(buffer, MAX_PACKET_SIZE);

    // Generate a job and queue it into a worker thread if the full 
    // message was received
    if(status.status_code == omdb::D_RECV_FULL)
    {
        job_number++;

        // TODO: Remove this, for debugging purposes only
        printf("Creating job number %d\n", job_number);

        // TODO: Properly parse buffer

        // Create the worker thread's task
        Job j = WorkThread::GenerateJob(job_number, buffer, this->data_store);
        Job* job_ptr = new (std::nothrow) Job(std::move(j));
        if(job_ptr == nullptr)
        {
            // Out of memory, crash?
            // std::terminate();
            printf("Out of memory! Unable to allocate job!\n");
            return true;
        }

        m_thread_results.push_back(job_ptr->get_future());
        m_job_to_connection[job_number] = conn;

        // Push the job to the thread
        WorkThreadData& thread_data = m_thread_data[GetAvailableThread()];

        while(!thread_data.job_queue.enqueue(job_ptr)) {}

        // Notify the thread to wake-up
        thread_data.cond_var->notify_one();

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

bool WorkManager::SendResult(omdb::Connection& conn, ResultBase* result)
{
    // Convert the result into a packet and then serialize into a buffer

    switch(result->type)
    {
        case ResultType::QUERY:
            // Query results turn into two packets: ResultMetaData and ResultData
            {
                ResultMetaDataPacket metadata_packet = {};
                ResultPacket result_packet = {};

                QueryResult* query = reinterpret_cast<QueryResult*>(result);

                // First, set up the metadata packet
                metadata_packet.type = PacketType::RESULT_METADATA;
                metadata_packet.status = query->status;

                // Only finish writing the packet if the result is successful
                if(query->status == ResultStatus::SUCCESS)
                {
                    metadata_packet.numColumns = query->result.metadata.size();
                    for(uint32_t itr = 0; itr < metadata_packet.numColumns; itr++)
                    {
                        metadata_packet.columns[itr].type = query->result.metadata[itr].type;
                        strcpy(metadata_packet.columns[itr].name,
                                query->result.metadata[itr].name);
                    }
                }
                metadata_packet.terminator = THE_TERMINATOR;

                // Then, the actual data packet
                result_packet.type = PacketType::RESULT_DATA;
                result_packet.status = metadata_packet.status;

                if(metadata_packet.status == ResultStatus::SUCCESS)
                {
                    result_packet.rowLen = metadata_packet.numColumns;

                    size_t result_size = result_packet.rowLen * query->result.data.size();
                    result_size *= sizeof(uint64_t);

                    result_packet.resultSize = result_size;

                    // TODO: Implement packet splitting to stay within packet maximums

                    uint64_t* result_data = new uint64_t[result_size/sizeof(uint64_t)];
                    uint32_t result_idx = 0;
                    for(auto record : query->result.data)
                    {
                        for(auto data : record)
                        {
                            // TODO: Mask off bottom 3 bits?
                            result_data[result_idx] = data.value;
                            result_idx++;
                        }
                    }
                    result_packet.data = result_data;
                    result_packet.terminator = THE_TERMINATOR;
                }

                // Now actually send the packets
                // TODO: Implement this
                assert(false);
            }
            break;
        case ResultType::COMMAND:
            break;
        default:
            // Shouldn't be reached
            printf("Unknown result reached %s\n", __FUNCTION__);
            break;
    }

    return false;
}
