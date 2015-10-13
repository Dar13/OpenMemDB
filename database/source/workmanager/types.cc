/**
 *
 */

// Standard library includes
#include <iostream>

// Project includes
#include <workmanager/types.h>

/**
 * @brief This is the framework in which the thread's jobs are executed in.
 *
 * @note This currently utilizes mutual exclusion to create a mechanism to pass these tasks
 *       to the thread, and this can be replaced with a Tervel FIFO data structure.
 */
void WorkThread::Run(WorkThreadData* data)
{
    // TODO: Remove the need for a mutex
    std::unique_lock<std::mutex> thread_lock(data->mutex, std::defer_lock);

    while(true)
    {
        thread_lock.lock();

        data->cond_var.wait(thread_lock, [data] ()
                {
                    if(data->stop)
                    {
                        return true;
                    }

                    if(!data->jobs.empty())
                    {
                        return true;
                    }

                    return false;
                });

        if(data->stop) { return; }

        Job do_job = std::move(data->jobs.front());
        data->jobs.pop();

        thread_lock.unlock();

        do_job(1);
    }
}

/**
 *  @brief Generates a task to be given to a thread.
 *
 *  @note This currently uses a lambda that binds the passed in string, we can
 *        instead use a wrapper or normal function.
 */
Job WorkThread::GenerateJob(int job_num, std::string command)
{
    Job job([job_num, command] (int test) -> Result
            {
                std::cout << job_num << " : ";
                std::cout << command << std::endl;

                Result res;
                res.job_number = job_num;
                res.result = test;

                return res;
            });

    return job;
}
