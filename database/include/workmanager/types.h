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

#ifndef WORKMGR_TYPES_H
#define WORKMGR_TYPES_H

// Standard library includes
#include <cstdint>
#include <thread>
#include <mutex>
#include <string>
#include <atomic>
#include <queue>
#include <future>
#include <condition_variable>

// Project includes
#include <data/types.h>
#include <util/types.h>
#include <util/result.h>
#include <util/packets.h>
#include <sql/common.h>

// Tervel includes
#include <tervel/util/tervel.h>
#include <tervel/containers/wf/linked_list_queue/queue.h>

struct JointResult
{
    JointResult(const ManipResult& result) :
        command(result)
    {}

    JointResult(const MultiRecordResult& result) :
        query(result)
    {}

    ManipResult command;
    MultiRecordResult query;
};

template<>
struct Result<JointResult> : public ResultBase
{
    Result(ResultStatus s, const ManipResult& res) :
        ResultBase(s, ResultType::COMMAND), result(res.result)
    {}

    Result(ResultStatus s, const MultiRecordResult& res) :
        ResultBase(s, ResultTypes::QUERY), result(res.result)
    {}

    JointResult result;
};

/**
 *  \brief Struct that relates a job number to its result
 */
struct JobResult
{
    JobResult() : job_number(0), result(nullptr) {}
    JobResult(uint32_t num, ResultBase* result = nullptr)
        : job_number(num), result(result)
    {}

    uint32_t job_number;    //!< The job number that's associated to this result
    ResultBase* result;     //!< Pointer to result
};

//! Typedef of a function pointer that returns a @refer Result and takes a pointer
typedef JobResult (*JobFunctor)(void);

//! Convenient typedef for the job concept.
typedef std::packaged_task<JobResult(void)> Job;

//! Convenient typedef for the Tervel queue class
template<typename T>
using TervelQueue = tervel::containers::wf::Queue<T>;

/**
 *  \brief A struct that holds some important objects needed to notify threads
 *  about various status changes they need to undergo, as directed by the 
 *  workmanager.
 */
struct ThreadNotifier
{
    ThreadNotifier()
        : used(false), stop(false)
    {}

    //! Whether the mutex is already assigned to a worker thread
    bool used;

    //! The variable the signifies an interruption of the thread
    std::atomic_bool stop;

    //! The mutex to be assigned to a thread
    std::mutex mutex;

    //! The condition variable to be used in conjunction with the mutex
    std::condition_variable cond_var;
};

/**
 *  @brief Additional data that allows for inter-thread communication.
 */
struct WorkThreadData
{
    WorkThreadData()
        : notifier(nullptr) 
    {}

    WorkThreadData(ThreadNotifier* notifier)
        : notifier(notifier) 
    {}

    WorkThreadData(WorkThreadData&& data)
        : tervel(data.tervel), thread(std::move(thread)), notifier(data.notifier)
    {}

    WorkThreadData& operator=(const WorkThreadData& other)
    {
        id = other.id;
        tervel = other.tervel;
        notifier = other.notifier;
    }

    //! Thread ID
    uint16_t id;

    //! Pointer to the tervel object
    tervel::Tervel* tervel;
    
    //! Thread object
    std::thread thread;

    //! Queue of jobs
    TervelQueue<Job*> job_queue;

    ThreadNotifier* notifier;
};

/**
 *  \brief Struct that holds the necessary data for composing the packets
 *  given back to the client that requested the query be run.
 */
struct QueryData
{
    //! Vector that holds the column information needed for a ResultMetadataPacket
    std::vector<ResultColumn> metadata;

    //! Vector of vectors of the data to be put into the ResultPacket
    std::vector<std::vector<TervelData>> data;
};

//! Convenience typedef for the result
typedef Result<QueryData> QueryResult;

//! Specialization of the template result to make sure its constructor is correct
template<>
struct Result<QueryData> : public ResultBase
{
    Result(ResultStatus s, QueryData res) : ResultBase(s, ResultType::QUERY), result(res) {}

    QueryData result;
};

#endif
