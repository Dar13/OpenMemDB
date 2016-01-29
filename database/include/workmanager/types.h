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
#include <queue>
#include <future>
#include <condition_variable>

// Project includes
#include <util/types.h>

// TODO: Make this into a true result type for SQL Query or Database command
typedef struct
{
    uint32_t job_number;    //!< The job number that's associated to this result
    uint64_t result;        // TODO: Make this into a real result
} JobResult;

//! Typedef of a function pointer that returns a @refer Result and takes an integer.
typedef JobResult (*JobFunctor)(int);

//! Convenient typedef for the job concept.
typedef std::packaged_task<JobResult(int)> Job;

/**
 *  @brief Additional data that allows for inter-thread communication.
 *
 *  @note TODO:The mutex and queue could probably be replaced with a Tervel data structure.
 */
struct WorkThreadData
{
    WorkThreadData()
        : stop(false)
    {}

    //! Thread ID
    uint32_t id;
    
    //! Thread object
    std::thread thread;

    //! Queue of jobs
    std::queue<Job> jobs;

    //! Condition variable, allows for waking up from sleep
    std::condition_variable cond_var;

    //! Mutex
    std::mutex mutex;

    //! If set, this thread should return immediately
    bool stop;
};

#endif
