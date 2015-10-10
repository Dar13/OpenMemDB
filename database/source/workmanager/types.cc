/**
 *
 */

// Standard library includes
#include <iostream>

// Project includes
#include <workmanager/types.h>

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

Job WorkThread::GenerateJob(int job_num, std::string command)
{
    Job job([job_num, command] (int test)
            {
                std::cout << job_num << " : ";
                std::cout << command << std::endl;

                return test;
            });

    return job;
}
