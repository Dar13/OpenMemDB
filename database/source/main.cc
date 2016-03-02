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

void createRow(std::vector<int64_t> value, RecordData &data);

/**
 *  @brief The entry point of the application.
 */
int main(int argc, char** argv)
{
    tervel::Tervel* tervel_test = new tervel::Tervel(8);
    tervel::ThreadContext* main_context = new tervel::ThreadContext(tervel_test);

    DataStore data;

    setupTokenMappings();

    //create table
    std::string create_table = "CREATE TABLE TestTable (A INTEGER, B INTEGER);";

    ParseResult parse_result = parse(create_table, &data);

    if(parse_result.status == ResultStatus::SUCCESS)
    {
        CreateTableCommand* create_table = reinterpret_cast<CreateTableCommand*>(parse_result.result);
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

	//TODO: TESTING SCHEMA HARDCODE IS DEFAULT AUTO_INCREMENT
	std::vector<int64_t> input = {0, 0};
    std::vector<int64_t> input2 = {0, 0};
	std::vector<int64_t> input3 = {-1, 0};

	//first row
    RecordData test_data;
    createRow(input, test_data);

    //second row
    RecordData test_data2;
    createRow(input2, test_data2);

	//third row
    RecordData test_data3;
    createRow(input3, test_data3);
	
    //inserting row 1 and row 2
    auto insert_err = data.insertRecord("TestTable", test_data);
    auto insert_err2 = data.insertRecord("TestTable", test_data2);
	auto insert_err3 = data.insertRecord("TestTable", test_data3);
    if(insert_err.status != ResultStatus::SUCCESS || insert_err2.status != ResultStatus::SUCCESS
		|| insert_err3.status != ResultStatus::SUCCESS)
    {
        printf("Failed to insert record\n");
    }
    else
    {
        printf("Record inserted\n");
    }

    std::string select_data = "SELECT TestTable.* FROM TestTable WHERE TestTable.A = 1;";
    parse_result = parse(select_data, &data);

    if(parse_result.status == ResultStatus::SUCCESS)
    {
        SelectQuery* query = reinterpret_cast<SelectQuery*>(parse_result.result);
        printf("Select parse success\n");

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

    return 1;

    /*
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

    */
}

//JUST FOR TESTING! DELETE AFTERWARDS
void createRow(std::vector<int64_t> value, RecordData& data)
{
    //RecordData data;

	for(auto itr: value)
	{
		TervelData t_d = { .value = 0 };
    	
        //null is represented as -1
        if(itr == -1)
        {
			t_d.data.type = INTEGER;
            t_d.data.value = itr;
			t_d.data.null = 1;
            printf("Inserting value: %ld\n", t_d.data.value);
            printf("Inserting congregate value: %ld\n", t_d.value);
            data.push_back(t_d);
        }
        else
        {
            t_d.data.type = INTEGER;
    	    t_d.data.value = itr;
    	    printf("Inserting value: %ld\n", t_d.data.value);
    	    printf("Inserting congregate value: %ld\n", t_d.value);
    	    data.push_back(t_d);
	    }
    }

	//return data;
}
