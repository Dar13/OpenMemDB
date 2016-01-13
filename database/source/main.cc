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

    setupTokenMappings();

    std::string test_select = "SELECT A.* FROM A WHERE A.x = 1 AND A.y = 2;";
    StatementBuilder select_parse = parse(test_select);
    if(select_parse.valid && select_parse.type == SQLStatement::SELECT)
    {
        printf("Select parsed!\n");
    }
    else
    {
        printf("Select not parsed!\n");
    }

    std::string create_table = "CREATE TABLE TestTable (A INTEGER, B INTEGER);";

    StatementBuilder parse_result = parse(create_table);

    if(parse_result.valid && parse_result.type == SQLStatement::CREATE_TABLE)
    {
        CreateTableCommand* create_table = reinterpret_cast<CreateTableCommand*>(parse_result.statement);
        auto err = data.createTable(*create_table);
        if(err.status == ResultStatus::SUCCESS)
        {
            printf("Table created\n");
        }
        else
        {
            printf("Unable to create table!\n");
        }
    }

    RecordData test_data;

    TervelData t_d = { .value = 0 };
    t_d.data.type = INTEGER;
    t_d.data.value = 1;
    printf("Inserting value: %ld\n", t_d.data.value);
    printf("Inserting congregate value: %ld\n", t_d.value);
    test_data.push_back(t_d);

    TervelData t_dd = { .value = 0 };
    t_dd.data.type = INTEGER;
    t_dd.data.value = 2;
    printf("Inserting value: %ld\n", t_dd.data.value);
    printf("Inserting congregate value: %ld\n", t_dd.value);
    test_data.push_back(t_dd);

    auto insert_err = data.insertRecord("TestTable", test_data);
    if(insert_err.status != ResultStatus::SUCCESS)
    {
        printf("Failed to insert record\n");
    }
    else
    {
        printf("Record inserted\n");
    }

    MultiRecordResult records = data.getRecords(nullptr, "TestTable");
    if(records.status == ResultStatus::SUCCESS)
    {
        for(auto record : records.result)
        {
            printf("Record:\n");
            for(auto data : record)
            {
                IntData int_data = { .value = data.data.value };
                printf(" - %ld\n", int_data.data);
            }
        }
    }
    else
    {
        printf("Unable to retrieve records\n");
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
