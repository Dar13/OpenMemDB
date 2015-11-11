/* TODO: File header */

#include <util/types.h>
#include <workmanager/types.h>

/**
 *  @brief Abstraction of a worker thread.
 *
 *  @note In its current implementation, the class is never actually instantiated.
 *  TODO: Replace with nested namespace?
 */
class WorkThread
{
public:
    static void Run(WorkThreadData* data);

    static Job GenerateJob(int job_num, std::string command);
private:
};
