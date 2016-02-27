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
#include <thread>
#include <chrono>

DataStoreTest::DataStoreTest(){}

// Will return TestResult (or DataStoreTestResult), int for placeholder
void DataStoreTest::createTest(std::vector<std::string> statements)
{

    tervel::Tervel* tervel_test = new tervel::Tervel(8);
    tervel::ThreadContext* main_context = new tervel::ThreadContext(tervel_test);

    DataStore data;

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

    printf("Success count: %d\n", successCount);

}

DataStoreTest& DataStoreTest::with(int mode)
{
    this->mode = mode;
    return *this;
}

DataStoreTest& DataStoreTest::generateCases(int testComplexity)
{
    complexity = testComplexity;
    parseComplexity(complexity);

    printf("%d\n", threadCount);

    switch(mode)
    {
        case MODE_CREATE:

            for(int i = 0; i < 7; i++)
            {
                std::string create_table = "CREATE TABLE TestT"+ std::to_string(i) +" (A STRING, B INTEGER);";
                statements.push_back(create_table);
            }

            break;
    }

    return *this;
}

TestResult DataStoreTest::test()
{

    switch(mode)
    {
        case MODE_CREATE:

            std::vector<std::thread> v;
            
            for (int i = 0; i < threadCount; ++i)
            {
                std::thread t1(&DataStoreTest::createTest, statements);
                v.push_back(std::move(t1));
            }

            auto time_start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < threadCount; ++i)
            {
                v.at(i).join();
            }

            auto time_end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start);

            TestResult testResult(duration.count(), threadCount);

            return testResult;

    }

    TestResult res(0ul, threadCount);
    return res;
}


/*
        Complexity bitmask:

        bit     purpose

        0       Enables random statement creation, useful for correctness testing
        1       Uses 2 threads for testing
        2       Uses 4 threads for testing
        3       Uses 8 threads for testing
*/
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