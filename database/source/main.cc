/*
 *
 */

#include <tervel/util/tervel.h>
#include <workmanager/work_manager.h>

/* TODO: REMOVE THIS, JUST FOR TESTING */
#include <sql/omdb_parser.h>
#include <data/data_store.h>

/**
 *  @brief The entry point of the application.
 */
int main(int argc, char** argv)
{
  setupTokenMappings();
  //parse("CREATE TABLE test (user_id INTEGER, is_admin BOOLEAN);");
  parse("SELECT A.* FROM A;");

  DataStore data;

  return 1;

  // TODO: Handle passed in parameters
  printf("Arguments passed in:\n");
  for(int i = 0; i < argc; i++)
  {
    printf("\t- %s\n", argv[i]);
  }

  tervel::Tervel* tervel_main = new tervel::Tervel(8);
  WorkManager work_manager(8, tervel_main);

  int32_t status = 0;

  status = work_manager.Initialize();
  if(status != WorkManager::E_NONE)
  {
    printf("Work manager failed to initialize!\n");
    printf("Error code = %d\n", status);
    return 1;
  }

  status = work_manager.Run();
  if(status != WorkManager::E_NONE)
  {
    printf("Work manager failed to run!\n");
    printf("Error code = %d\n", status);
    return 1;
  }

  printf("Shutting down program...\n");

  return 0;
}
