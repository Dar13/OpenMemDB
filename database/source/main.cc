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
    tervel::Tervel* tervel_test = new tervel::Tervel(8);
    tervel::ThreadContext* main_context = new tervel::ThreadContext(tervel_test);
    DataStore data;

    CreateTableCommand test_table;
    test_table.table_name = "TestTable";

    SQLColumn a_col;
    a_col.name = "A";
    a_col.type = INTEGER;
    a_col.constraint.type = SQLConstraintType::SQL_NO_CONSTRAINT;

    SQLColumn b_col;
    b_col.name = "B";
    b_col.type = INTEGER;
    b_col.constraint.type = SQLConstraintType::SQL_NO_CONSTRAINT;

    test_table.columns.push_back(a_col);
    test_table.columns.push_back(b_col);

    printf("Tervel initialized, test CREATE TABLE setup\n");

    auto err = data.createTable(test_table);
    if(err.status != ResultStatus::SUCCESS)
    {
	printf("Failed to create table\n");
    }
    else
    {
	printf("Empty table created\n");
    }

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
