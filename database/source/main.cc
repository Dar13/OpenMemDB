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

    std::string create_table = "CREATE TABLE TestTable (A INTEGER, B INTEGER);";

    ParseResult parse_result = parse(create_table, &data);

    if(parse_result.status == ResultStatus::SUCCESS)
    {
        std::unique_ptr<CreateTableCommand> create_table(reinterpret_cast<CreateTableCommand*>(parse_result.result));
        //CreateTableCommand* create_table = reinterpret_cast<CreateTableCommand*>(parse_result.result);
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

    /*
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
    */

	std::string insert_test = "INSERT INTO TestTable VALUES (1, 2);";
	auto insert_parse = parse(insert_test, &data);
	if(insert_parse.status == ResultStatus::SUCCESS)
	{
		printf("INSERT INTO statement parsed!\n");
        std::unique_ptr<InsertCommand> cmd(reinterpret_cast<InsertCommand*>(insert_parse.result));

        printf("Inserting into %s\n", cmd->table.c_str());
        printf("Data to be inserted: ");
        for(data : cmd->data)
        {
            printf("%lu, ", data.data.value);
        }
        printf("\n");

        auto insert_err = data.insertRecord(cmd->table, cmd->data);
        if(insert_err.status != ResultStatus::SUCCESS)
        {
            printf("Failed to insert record into Data Store!\n");
        }
        else
        {
            printf("Record inserted!\n");
        }
	}
	else
	{
		printf("INSERT INTO statement failed to parse!\n");
	}

    std::string select_data = "SELECT TestTable.* FROM TestTable WHERE TestTable.A = 1 OR TestTable.B = 2;";
    parse_result = parse(select_data, &data);

    if(parse_result.status == ResultStatus::SUCCESS)
    {
        printf("Select parse success\n");
        std::unique_ptr<SelectQuery> query(reinterpret_cast<SelectQuery*>(parse_result.result));

        auto select_result = data.getRecords(query->predicate, std::string("TestTable"));
        if(select_result.status == ResultStatus::SUCCESS)
        {
            printf("SELECT is successful!\n");
            printf("Rows retrieved: %zu\n", select_result.result.size());
            for(RecordData record : select_result.result)
            {
                for(TervelData data : record)
                {
                    printf("%lu,", data.data.value);
                }
                printf("\n");
            }
        }
        else
        {
            printf("SELECT failed!\n");
        }
    }
    else
    {
        printf("Select parse failed\n");
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

    std::string drop_table = "DROP TABLE TestTable;";
    auto drop_parse_result = parse(drop_table, &data);
    if(drop_parse_result.status == ResultStatus::SUCCESS)
    {
        printf("Drop table statement parsed!\n");
        std::unique_ptr<DropTableCommand> cmd(reinterpret_cast<DropTableCommand*>(drop_parse_result.result));
        data.deleteTable(cmd->table_name);
    }
    else
    {
        printf("Drop table statement didn't successfully parse!\n");
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
