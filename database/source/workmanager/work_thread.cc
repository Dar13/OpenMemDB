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

#include <workmanager/types.h>
#include <workmanager/work_thread.h>

// Tervel include
#include <tervel/util/tervel.h>

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

    // Condition variables require an unique_lock
    std::unique_lock<std::mutex> thread_lock(*data->mutex, std::defer_lock);

    while(true)
    {
        thread_lock.lock();

        // TODO: try/catch block needed?
        while(!data->cond_var->wait_for(thread_lock, std::chrono::milliseconds(15),
                [&data] () -> bool
                {
                    return (data->stop->load() || !data->job_queue.empty());
                })) {}
        thread_lock.unlock();

        if(data->stop->load()) { break; }

        Job* job = nullptr;
        TervelQueue<Job*>::Accessor queue_getter;
        if(data->job_queue.dequeue(queue_getter))
        {
            uintptr_t job_ptr = reinterpret_cast<uintptr_t>(queue_getter.value());
            // Mask off the bottom 4 bits, as Tervel modified them
            job_ptr = job_ptr & (~0xF);
            job = reinterpret_cast<Job*>(job_ptr);
        }
        else
        {
            continue;
        }

        if(job != nullptr)
        {
            job->operator()(1);
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
Job WorkThread::GenerateJob(int job_num, std::string command)
{
    Job job([job_num, command] (int test) -> JobResult
            {
                JobResult res;
                res.job_number = job_num;

                printf("Command: %s\n", command.c_str());

                // Execute command/query and generate a suitable result.

                res.result = new Uint64Result(ResultStatus::SUCCESS, 1);

                return res;
            });

    return job;
}
