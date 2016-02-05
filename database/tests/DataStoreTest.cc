#include "DataStoreTest.h"
#include <data/data_store.h>
#include <sql/omdb_parser.h>
#include "SQLGenerator.h"

#include <vector>
#include <stdio.h>
#include <string>

DataStoreTest::DataStoreTest(){}


// Will return TestResult (or DataStoreTestResult), int for placeholder
int DataStoreTest::createTest()
{

	SQLGenerator sqlGenerator;
	DataStore data;

    setupTokenMappings();


	std::string create_table = sqlGenerator.generateCreateStatement(0);
    ParseResult parse_result = parse(create_table, &data);

    if(parse_result.status == ResultStatus::SUCCESS)
    {

        CreateTableCommand* create_table = reinterpret_cast<CreateTableCommand*>(parse_result.result);
        auto err = data.createTable(*create_table);
    	printf("test\n");
        if(err.status == ResultStatus::SUCCESS)
        {
            printf("Table created\n");
        }
        else
        {
            printf("Unable to create table!\n");
        }
    }

	return 1;

}