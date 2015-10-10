/**
 *
 */

// Project includes
#include <workmanager/work_manager.h>

WorkManager::WorkManager(uint32_t num_threads, tervel::Tervel* tervel)
    : m_tervel(tervel)
{
}

int32_t WorkManager::Initialize()
{
    std::cout << "Initializing stuff\n";

    uint32_t thread_id = 0;
    for(auto& thread_data : m_thread_data)
    {
        thread_data.id = thread_id;
        thread_id++;

        thread_data.stop = false;

        thread_data.thread = std::thread(WorkThread::Run, &thread_data);
    }

    return E_NONE;
}

int32_t WorkManager::Run()
{
    std::cout << "Running WorkManager::Run()\n";

    int job_num = 0;
    for(auto& thread_data : m_thread_data)
    {
        for(int i = 0; i < 2; i++)
        {
           Job j = WorkThread::GenerateJob(job_num, "Hello World!"); 
           m_thread_results.push_back(j.get_future());
           job_num++;

           std::unique_lock<std::mutex> lock(thread_data.mutex);
           thread_data.jobs.push(std::move(j));
        }

        thread_data.cond_var.notify_one();
    }

    bool waiting = true;
    while(waiting)
    {
        bool all_done = true;
        for(auto& result : m_thread_results)
        {
            if(result.valid() && 
               result.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                std::cout << "Job num " << result.get();
                std::cout << " is done!" << std::endl;
            }
            else
            {
                if(result.valid())
                {
                    all_done = false;
                }
            }
        }

        waiting = (!all_done);
    }

    std::cout << "All done!" << std::endl;

    return E_NONE;
}
