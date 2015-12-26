/**
 *
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
