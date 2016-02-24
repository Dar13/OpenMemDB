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
#include <util/types.h>
#include <util/result.h>
#include <util/libomdb.h>
#include <sql/common.h>

// Tervel includes
#include <tervel/util/tervel.h>
#include <tervel/containers/wf/linked_list_queue/queue.h>

// TODO: Document this
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
 *  \brief TODO
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
        : stop(nullptr), cond_var(nullptr), mutex(nullptr)
    {}

    WorkThreadData(ThreadNotifier* notifier)
        : stop(&(notifier->stop)), cond_var(&notifier->cond_var), 
        mutex(&notifier->mutex)
    {}

    WorkThreadData(WorkThreadData&& data)
        : tervel(data.tervel), stop(data.stop), thread(std::move(thread)), 
        cond_var(data.cond_var), mutex(data.mutex)
    {}

    //! Thread ID
    uint16_t id;

    //! Pointer to the tervel object
    tervel::Tervel* tervel;

    //! If set, this thread should return immediately
    std::atomic_bool* stop;
    
    //! Thread object
    std::thread thread;

    //! Queue of jobs
    TervelQueue<Job*> job_queue;

    //! Condition variable, allows for waking up from sleep
    std::condition_variable* cond_var;

    //! Mutex
    std::mutex* mutex;
};

struct QueryData
{
    std::vector<ResultColumn> metadata;
    std::vector<std::vector<TervelData>> data;
};

typedef Result<QueryData> QueryResult;

template<>
struct Result<QueryData> : public ResultBase
{
    Result(ResultStatus s, QueryData res) : ResultBase(s, ResultType::QUERY), result(res) {}

    QueryData result;
};

#endif
