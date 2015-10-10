/*
 *
 */

#include <workmanager/work_manager.h>

/**
 *  @brief The entry point of the application.
 */
int main(int argc, char** argv)
{
    WorkManager work_manager(8, nullptr);

    work_manager.Initialize();

    work_manager.Run();

    printf("Done!\n");

    return 0;
}
