#include <data/data_store.h>
#include <sql/omdb_parser.h>
#include <tervel/util/tervel.h>
#include <workmanager/work_manager.h>
#include "DataStoreTest.h"
#include "SQLGenerator.h"
#include "TestResult.h"
#include "Modes.h"

#include <vector>
#include <stdio.h>
#include <string>

/*

        Complexity bitmask:

        bit     purpose

        0       Enables random statement creation, useful for correctness testing
        1       Uses 2 threads for testing
        2       Uses 4 threads for testing
        3       Uses 8 threads for testing
*/

bool isRandomized;
int threadCount;
std::vector<std::string> statements;


DataStoreTest::DataStoreTest(){}

// Will return TestResult (or DataStoreTestResult), int for placeholder
int DataStoreTest::createTest(int complexity)
{

    tervel::Tervel* tervel_test = new tervel::Tervel(8);
    tervel::ThreadContext* main_context = new tervel::ThreadContext(tervel_test);

    SQLGenerator sqlGenerator;
    DataStore data;

    if(isRandomized){
        printf("test\n");
    }

    printf("%d\n",threadCount);
    setupTokenMappings();

    int successCount = 0;

    for(auto i = statements.begin(); i  != statements.end(); i++)
    {    
        ParseResult parse_result = parse(*i, &data);

        if(parse_result.status == ResultStatus::SUCCESS)
        {
            CreateTableCommand* create_table = reinterpret_cast<CreateTableCommand*>(parse_result.result);
            auto err = data.createTable(*create_table);
            if(err.status == ResultStatus::SUCCESS)
            {
                printf("Table created\n");
                successCount++;

            }
            else
            {
                printf("Unable to create table!\n");
            }
        }
    }

    printf("%d\n", successCount);
	return 1;

}

void DataStoreTest::parseComplexity(int complexity)
{
    isRandomized = complexity & 0b0001;

    if(complexity & 0b0010)
    {   
        threadCount = 2;
    }
    else if(complexity & 0b0100)
    {
        threadCount = 4;
    }
    else if(complexity & 0b1000)
    {
        threadCount = 8;
    }
    else
    {
        threadCount = 1;
    }
}

DataStoreTest& DataStoreTest::with(int mode)
{
    testMode = mode;
    return *this;
}

DataStoreTest& DataStoreTest::generateCases(int testComplexity)
{
    complexity = testComplexity;
    parseComplexity(complexity);


    switch(testMode)
    {
        case MODE_CREATE:


            for(int i = 0; i < 10; i++)
            {
                std::string create_table = "CREATE TABLE TestTable (A STRING, B INTEGER);";
                statements.push_back(create_table);
            }

            break;

    }
    return *this;
}

DataStoreTest& DataStoreTest::test()
{

    switch(testMode)
    {
        case MODE_CREATE:
            createTest(complexity);
    }


    return *this;
}