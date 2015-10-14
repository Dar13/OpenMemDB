/*
 *
 */

#include <workmanager/work_manager.h>

/**
 *  @brief The entry point of the application.
 */
int main(int argc, char** argv)
{
    // TODO: Integrate Tervel
    WorkManager work_manager(8, nullptr);

    int32_t status = 0;

    // TODO: Error handling
    status = work_manager.Initialize();

    // TODO: Error handling
    status = work_manager.Run();

    printf("Shutting down program...\n");

    return 0;
}
