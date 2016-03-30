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

// Project includes
#include <data/data_store.h>
#include <sql/omdb_parser.h>
#include <workmanager/types.h>
#include <workmanager/work_thread.h>

// Tervel include
#include <tervel/util/tervel.h>

// Stdlib include
#include <cstring>

/**
 * @brief This is the framework in which the thread's jobs are executed in.
 *
 * @note This currently utilizes mutual exclusion to create a mechanism to pass these tasks
 *       to the thread, and this can be replaced with a Tervel FIFO data structure.
 */
void WorkThread::Run(WorkThreadData* data)
{
    // If this throws, then we're dead in the water anyways. Let it crash
    tervel::ThreadContext* thread_context = new tervel::ThreadContext(data->tervel);

    ThreadNotifier* notifier = data->notifier;

    // All threads must run this
    setupTokenMappings();

    // Condition variables require an unique_lock
    std::unique_lock<std::mutex> thread_lock(notifier->mutex, std::defer_lock);

    while(true)
    {
        thread_lock.lock();

        // TODO: try/catch block needed?
        while(!notifier->cond_var.wait_for(thread_lock, std::chrono::milliseconds(15),
                [&data, notifier] () -> bool
                {
                    return (notifier->stop.load() || !data->job_queue.empty());
                })) {}
        thread_lock.unlock();

        if(notifier->stop.load()) { break; }

        Job* job = nullptr;
        TervelQueue<Job*>::Accessor queue_getter;
        if(data->job_queue.dequeue(queue_getter))
        {
            uintptr_t job_ptr = reinterpret_cast<uintptr_t>(queue_getter.value());
            // Mask off the bottom 3 bits, as Tervel likely modified them.
            // Malloc/new on Linux guarantees that the address will be a
            // multiple of 16
            job_ptr = job_ptr & (~0x7);
            job = reinterpret_cast<Job*>(job_ptr);
        }
        else
        {
            continue;
        }

        if(job != nullptr)
        {
            // Execute the job
            job->operator()();
            delete job;
        }
    }

    delete thread_context;
}

/**
 *  @brief Generates a task to be given to a thread.
 *
 *  @note This currently uses a lambda that binds the passed in string, we can
 *        instead use a wrapper or normal function.
 */
Job WorkThread::GenerateJob(int job_num, std::string command, DataStore* store)
{
    Job job([job_num, command, store] (void) -> JobResult
            {
                JobResult res(job_num);

                if(store == nullptr)
                {
                    res.result = new ManipResult(ResultStatus::FAILURE_DB_UNKNOWN_STATE,
                            ManipStatus::ERR);

                    // No point going any further, the database is in an
                    // unknown state.
                    return res;
                }

                // TODO: Remove this at some point
                printf("Command: %s\n", command.c_str());

                // Parse the command
                auto parse_result = parse(command, store);

                // Check if the parse succeeded and the statement is non-null
                // (trust, but verify)
                if(parse_result.status == ResultStatus::SUCCESS &&
                    parse_result.result != nullptr)
                {
                    res.result = ExecuteStatement(parse_result.result, store);
                }
                else
                {
                    // Propagate error back to work manager
                    res.result = new ManipResult(ResultStatus::FAILURE_SYNTAX,
                            ManipStatus::ERR);
                }

                printf("Returning from job #%d\n", job_num);

                return res;
            });

    return job;
}

/**
 *  \brief Executes the given statement on the data store object, returning a 
 *  result.
 */
ResultBase* WorkThread::ExecuteStatement(ParsedStatement* statement, DataStore* store)
{
    ResultBase* result = nullptr;
    switch(statement->type)
    {
        // Commands
        case SQLStatement::CREATE_TABLE:
        case SQLStatement::DROP_TABLE:
        case SQLStatement::UPDATE:
        case SQLStatement::INSERT_INTO:
        case SQLStatement::DELETE:
            {
                ManipResult statement_result = ExecuteCommand(statement, store);
                // TODO: Make this a move constructor for performance
                result = new (std::nothrow) ManipResult(statement_result);
                if(result == nullptr)
                {
                    // Uhh, what now?
                    // Crash for now
                    std::terminate();
                    return nullptr;
                }

                return result;
            }
            break;
        // Query
        case SQLStatement::SELECT:
            {
                SelectQuery* query = reinterpret_cast<SelectQuery*>(statement);
                MultiRecordResult statement_result = ExecuteQuery(statement, store);
                if(statement_result.status != ResultStatus::SUCCESS)
                {
                    // TODO: Error handling
                    assert(false);
                }

                // Extract type and data from the query result, and the column
                // names from the query statement itself
                QueryData query_data;
                if(statement_result.result.size() == 0)
                {
                    // Just copy over the metadata, leave result empty
                    for(auto column : query->output_columns)
                    {
                        ResultColumn res_column;
                        strcpy(res_column.name, column.c_str());
                        res_column.type = static_cast<uint16_t>(DataType::INTEGER);

                        query_data.metadata.push_back(res_column);
                    }

                    query_data.data.clear();

                    return new (std::nothrow) QueryResult(ResultStatus::SUCCESS, query_data);
                }

                // Grab first record to get metadata
                RecordData& record = statement_result.result.front();
                size_t record_size = record.size();

                // Iterate over the record and grab the necessary metadata
                for(uint32_t itr = 0; itr < record_size; itr++)
                {
                    ResultColumn column;
                    TervelData& data = record[itr];
                    column.type = static_cast<uint16_t>(data.data.type);

                    std::string& col_name = query->output_columns[itr];
                    if((col_name.length() + 1) < sizeof(column.name))
                    {
                        // Assumes chars are 1 byte
                        strcpy(column.name, col_name.c_str());
                    }
                    else
                    {
                        // Invalid length output column
                        // Probably should be caught before this
                        // Truncate?
                        // TODO: Handle this
                        assert(false);
                    }

                    query_data.metadata.push_back(column);
                }

                // TODO: Enforce this being a move rather than a copy
                query_data.data = statement_result.result;
                for(auto r_data : query_data.data)
                {
                    for(auto t_data : r_data)
                    {
                        printf("%lu, ", t_data.value);
                    }
                    printf("\n");
                }

                QueryResult* query_result = new (std::nothrow) QueryResult(ResultStatus::SUCCESS,
                                                                            query_data);
                if(query_result == nullptr)
                {
                    // What do?
                    assert(false);
                    return nullptr;
                }
                
                return query_result;
            }
            break;
        // Invalid or unknown statement, don't attempt to execute
        default:
            // Might as well give them an error code and continue to run
            // This code should never be run though
            return new (std::nothrow) ManipResult(ResultStatus::FAILURE,
                    ManipStatus::ERR_UNKNOWN_STATEMENT);
            break;
    }

    return result;
}

/**
 *  \brief Executes a command statement on the data store object given and returns a
 *  relevant result.
 */
ManipResult WorkThread::ExecuteCommand(ParsedStatement* statement, DataStore* store)
{
    switch(statement->type)
    {
        case SQLStatement::CREATE_TABLE:
            return store->createTable(*reinterpret_cast<CreateTableCommand*>(statement));
            break;
        case SQLStatement::DROP_TABLE:
            return store->deleteTable(reinterpret_cast<DropTableCommand*>(statement)->table_name);
            break;
        case SQLStatement::UPDATE:
            {
                UpdateCommand* update_cmd = reinterpret_cast<UpdateCommand*>(statement);
                if(update_cmd->runnable)
                {
                    return store->updateRecords(update_cmd->predicate, update_cmd->table, 
                            update_cmd->full_record);
                }
                else
                {
                    return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR_TABLE_NOT_EXIST);
                }
            }
            break;
        case SQLStatement::INSERT_INTO:
            {
                InsertCommand* insert_cmd = reinterpret_cast<InsertCommand*>(statement);
                return store->insertRecord(insert_cmd->table, insert_cmd->data);
            }
            break;
        case SQLStatement::DELETE:
            {
                DeleteCommand* delete_cmd = reinterpret_cast<DeleteCommand*>(statement);
                return store->deleteRecords(delete_cmd->predicate, delete_cmd->table);
            }
            break;
        default:
            // Should never be hit
            break;
    }

    // Shouldn't be hit at all, but will when the command isn't implemented
    return ManipResult(ResultStatus::FAILURE, ManipStatus::ERR_UNKNOWN_STATEMENT);
}

/**
 *  \brief Executes a query statement (i.e. SELECT) on the data store object given
 *  and returns a relevant result
 */
MultiRecordResult WorkThread::ExecuteQuery(ParsedStatement* statement, DataStore* store)
{
    // Only one type of query, no need for a switch
    // Verify the type of statement is correct though
    if(statement == nullptr)
    {
        printf("Parsed statement is null!\n");
    }

    if(statement->type != SQLStatement::SELECT)
    {
        return MultiRecordResult(ResultStatus::FAILURE, MultiRecordData());
    }

    SelectQuery* query = reinterpret_cast<SelectQuery*>(statement);

    for(auto table : query->tables)
    {
        printf("Table: %s\n", table.c_str());
    }

    return store->getRecords(query->predicate, query->tables.front());
}
