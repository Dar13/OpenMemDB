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
#include <util/common.h>
#include <workmanager/work_manager.h>
#include <workmanager/work_thread.h>
#include <data/data_store.h>

/**
 *  \brief Constructor that initializes internal variables properly.
 */
WorkManager::WorkManager(uint32_t num_threads, tervel::Tervel* tervel)
    : m_tervel(tervel), m_num_threads(num_threads), m_thread_data(num_threads)
{
    m_context = new tervel::ThreadContext(m_tervel);
    this->m_data_store = new DataStore();
}

/**
 *  \brief Destructor that releases managed resources properly.
 */
WorkManager::~WorkManager()
{
    if(m_context)
        delete m_context;
    
    if(m_data_store)
        delete m_data_store;
}

/**
 *  \brief Initialize the internal data structures and members, prepare to 
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
        data.id = thread_id;
        data.tervel = m_tervel;

        // Have to set the thread after the copy into the vector so that it's a 
        // proper move rather than a copy(std::thread can't be copied)
        m_thread_data[thread_id] = data;
        m_thread_data[thread_id].thread = std::thread(WorkThread::Run, &m_thread_data[thread_id]);
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
 *  \brief Start the processing of client requests/commands, this method is 
 *         essentially starting the database server.
 */
int32_t WorkManager::Run()
{
    std::cout << "Running WorkManager::Run()\n";

    omdb::NetworkStatus status;

    // Here's where we store the results
    std::vector<JobResult> results;

    // Quick lambda for use in std::remove_if()
    // Checks for the future to be finished, returns true if it is.
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

    // The main loop of the application
    bool running = true;
    while(running)
    {
        uint32_t conn_socket_fd = 0;

        sockaddr_storage conn_addr;
        memset(&conn_addr, 0, sizeof(sockaddr_storage));

        // Check for a waiting connection
        status = omdb::AcceptConnection(m_server_socket_fd,
                                        &conn_socket_fd,
                                        &conn_addr);

        // Check for errors
        if(status.status_code != E_NONE)
        {
            printf("Error accepting connection\n");
            printf("Code: %d Secondary: %s\n", status.status_code,
                                                strerror(status.secondary_code));
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

            if(!SendResult(conn, res.result))
            {
                printf("Unable to send job!\n");
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
 *  \brief Handle incoming commands from client connections.
 */
bool WorkManager::ReceiveCommand(omdb::Connection& conn)
{
    char command_buffer[sizeof(CommandPacket)];
    
    // We're making this unsigned so the overflow behavior is predictable
    // If we ever get more than 3 billion concurrent requests, we'll be in trouble though...
    static uint32_t job_number = 0;

    omdb::NetworkStatus status;

    // Is there anything in the queue?
    status = conn.recv(command_buffer, sizeof(CommandPacket));

    // Generate a job and queue it into a worker thread if the full 
    // message was received
    if(status.status_code == omdb::D_RECV_FULL)
    {
        job_number++;

        // TODO: Remove this, for debugging purposes only
        printf("Creating job number %d\n", job_number);

        CommandPacket command;
        memcpy(&command, command_buffer, sizeof(CommandPacket));

        // Create the worker thread's task
        Job j = WorkThread::GenerateJob(job_number, command.message, this->m_data_store);
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
        thread_data.notifier->cond_var.notify_one();

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

bool WorkManager::SendResult(omdb::Connection& conn, const JointResult& result)
{
    // Convert the result into a packet and then serialize into a buffer

    switch(result.type)
    {
        case ResultType::QUERY:
            // Query results turn into two packets: ResultMetaData and ResultData
            {
                ResultMetaDataPacket metadata_packet = {};
                ResultPacket result_packet = {};

                const QueryResult& query = result.query_result;

                // First, set up the metadata packet
                metadata_packet.type = PacketType::RESULT_METADATA;
                metadata_packet.status = query.status;

                // Only finish writing the packet if the result is successful
                if(query.status == ResultStatus::SUCCESS)
                {
                    metadata_packet.numColumns = query.result.metadata.size();
                    for(uint32_t itr = 0; itr < metadata_packet.numColumns; itr++)
                    {
                        metadata_packet.columns[itr].type = query.result.metadata[itr].type;
                        strcpy(metadata_packet.columns[itr].name,
                                query.result.metadata[itr].name);
                    }
                }
                metadata_packet.terminator = THE_TERMINATOR;

                // Then, the actual data packet
                result_packet.type = PacketType::RESULT_DATA;
                result_packet.status = metadata_packet.status;

                if(metadata_packet.status == ResultStatus::SUCCESS)
                {
                    result_packet.rowLen = metadata_packet.numColumns;

                    size_t result_size = result_packet.rowLen * query.result.data.size();
                    result_size *= sizeof(uint64_t);

                    result_packet.resultSize = result_size;

                    // TODO: Implement packet splitting to stay within packet maximums

                    uint64_t* result_data = new uint64_t[result_size/sizeof(uint64_t)];
                    memset(result_data, 0, result_size / sizeof(uint64_t));

                    uint32_t result_idx = 0;
                    for(auto record : query.result.data)
                    {
                        for(auto data : record)
                        {
                            data.data.tervel_status = 0;
                            result_data[result_idx] = data.value;
                            result_idx++;
                        }
                    }
                    result_packet.data = result_data;
                    result_packet.terminator = THE_TERMINATOR;
                }
                
                // TODO: Verify
                metadata_packet.resultPacketSize = 9 + result_packet.resultSize + 1;

                // Serialize the packets
                // TODO: Convert to static buffer
                char* metadata_buffer = new char[sizeof(ResultMetaDataPacket)];
                serializeMetaDataPacket(metadata_packet, metadata_buffer);

                char* result_buffer = new char[metadata_packet.resultPacketSize];
                serializeResultPacket(result_packet, result_buffer);

                // Now actually send the data
                auto net_status = conn.send(metadata_buffer, sizeof(ResultMetaDataPacket));
                if(net_status.status_code != omdb::NetworkStatusCodes::SUCCESS &&
                    net_status.status_code != omdb::NetworkStatusCodes::D_SEND_FULL)
                {
                    // TODO: Handle error
                    printf("Actual error sending metadata for query\n");
                    return false;
                }

                net_status = conn.send(result_buffer, metadata_packet.resultPacketSize);
                if(net_status.status_code != omdb::NetworkStatusCodes::SUCCESS &&
                    net_status.status_code != omdb::NetworkStatusCodes::D_SEND_FULL)
                {
                    // TODO: Handle error
                    printf("Actual error sending data for query\n");
                    return false;
                }

                delete metadata_buffer;
                delete result_buffer;
            }
            break;
        case ResultType::COMMAND:
            {
                ResultMetaDataPacket metadata_packet = {};
                ResultPacket result_packet = {};

                const ManipResult& cmd_result = result.command_result;

                metadata_packet.type = PacketType::RESULT_METADATA;
                metadata_packet.status = cmd_result.status;
                metadata_packet.numColumns = static_cast<uint32_t>(cmd_result.result);

                // Leaving the columns set to 0, they're unused

                // Tiny packet
                metadata_packet.resultPacketSize = 10;

                metadata_packet.terminator = THE_TERMINATOR;

                // Setup the metadata packet
                result_packet.type = PacketType::RESULT_DATA;
                result_packet.status = metadata_packet.status;
                result_packet.resultSize = cmd_result.rows_affected;
                result_packet.terminator = THE_TERMINATOR;
                // All other fields should be set to zero
                
                char* metadata_buffer = new char[sizeof(ResultMetaDataPacket)];
                serializeMetaDataPacket(metadata_packet, metadata_buffer);

                char* result_buffer = new char[metadata_packet.resultPacketSize];
                serializeResultPacket(result_packet, result_buffer);

                // Now actually send the data
                auto net_status = conn.send(metadata_buffer, sizeof(ResultMetaDataPacket));
                if(net_status.status_code != omdb::NetworkStatusCodes::SUCCESS &&
                    net_status.status_code != omdb::NetworkStatusCodes::D_SEND_FULL)
                {
                    // TODO: Handle error
                    printf("Failed sending result metadata packet!\n");
                    printf("Error code: %d\n", net_status.status_code);
                    return false;
                }

                net_status = conn.send(result_buffer, metadata_packet.resultPacketSize);
                if(net_status.status_code != omdb::NetworkStatusCodes::SUCCESS &&
                    net_status.status_code != omdb::NetworkStatusCodes::D_SEND_FULL)
                {
                    // TODO: Handle error
                    printf("Failed sending result packet!\n");
                    printf("Error code: %d\n", net_status.status_code);
                    return false;
                }

                // TODO: Remove when switched to static buffer
                delete metadata_buffer;
                delete result_buffer;
            }
            break;
        default:
            // Shouldn't be reached
            printf("%s: Unknown result returned from query executor!\n", __FUNCTION__);
            break;
    }

    return true;
}

void WorkManager::Abort()
{
    // Perform emergency clean-up
    close(m_server_socket_fd);

    // TODO: Others?
}
