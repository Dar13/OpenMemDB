#include <data/data_store.h>
#include <sql/omdb_parser.h>
#include <tervel/util/tervel.h>
#include <workmanager/work_manager.h>
#include "DataStoreTest.h"
#include "SQLGenerator.h"
#include "TestResult.h"
#include "Modes.h"

#include "TestConstants.h"

#include <vector>
#include <stdio.h>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <tuple>

DataStoreTest::DataStoreTest()
{
}

// Will return TestResult (or DataStoreTestResult), int for placeholder
void DataStoreTest::createTest(std::vector<std::string> statements, void *t_data)
{
    struct thread_data *grab;
    grab = (struct thread_data *) t_data;
    int successCount = 0;

    setupTokenMappings();

    tervel::Tervel* tervel_test = grab->tervel_test;
    tervel::ThreadContext* main_context = new tervel::ThreadContext(tervel_test);
    DataStore *data = grab->data;

    // Execute create table commands from statements vector (defined in h file)
    for(auto i = statements.begin(); i  != statements.end(); i++)
    {

        ParseResult parse_result = parse(*i, data);

        if(parse_result.status == ResultStatus::SUCCESS)
        {
            CreateTableCommand* create_table = reinterpret_cast<CreateTableCommand*>(parse_result.result);
            auto err = data->createTable(*create_table);
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

    delete main_context;
}

void DataStoreTest::dropTest(std::vector<std::string> table_name, void *t_data)
{
    struct thread_data *grab;
    grab = (struct thread_data *) t_data;

    tervel::Tervel* tervel_test = grab->tervel_test;
    tervel::ThreadContext* main_context = new tervel::ThreadContext(tervel_test);
    DataStore *data = grab->data;

    //Drop tables after creating them
    for(auto i = table_name.begin(); i != table_name.end(); i++)
    {
        auto err = data->deleteTable(*i);
        if(err.status == ResultStatus::SUCCESS)
        {
            std::cout << "Table " << *i << " deleted" << std::endl;
        }
        else
        {
            printf("Unable to delete table\n");
        }
    }

    delete main_context;
}


void DataStoreTest::insertTest(std::vector<int64_t> record, void *t_data)
{
    struct thread_data *grab;
    grab = (struct thread_data *) t_data;

    tervel::Tervel* tervel_test = grab->tervel_test;
    tervel::ThreadContext* main_context = new tervel::ThreadContext(tervel_test);    
    DataStore *data = grab->data;

    std::vector<TervelData> table_row;
    for(int i = 0; i < record.size(); i++)
    {
        TervelData new_int = {.value = 0};

        new_int.data.tervel_status = 0;
        new_int.data.null = 0;
        new_int.data.value = record.at(i);

        //TODO: Would null integer be null? How to set null flag...

        table_row.push_back(new_int);
        auto err = data->insertRecord("TestT0", table_row);
        if(err.status != ResultStatus::SUCCESS)
        {
            printf("Unable to insert record\n");
        }
    }

    delete main_context;
}

// First method that should be called when making a test, passes in just the type of test. 
// Later on we will have the ability to set your own sql strings, this will also be passed in here
DataStoreTest& DataStoreTest::with(int mode)
{
    this->mode = mode;
    return *this;
}

// Depending on the complexity, it will make test cases with little variance (no randomization)
// or high variance (for correctness testing)
DataStoreTest& DataStoreTest::generateCases(int testComplexity)
{
    complexity = testComplexity;
    parseComplexity(complexity);
    share.tervel_test = new tervel::Tervel(2*threadCount);


    printf("Thread count: %d\n", threadCount);

    switch(mode)
    {
        case MODE_CREATE:
        {
            for(int i = 0; i < TestConstants::MaxTables; i++)
            {
                std::string create_table = "CREATE TABLE TestT"+ std::to_string(i) +" (A STRING, B INTEGER);";
                statements.push_back(create_table);
            }
            break;
        }

        case MODE_DROP:
        {

            for(int i = 0; i < TestConstants::MaxTables; i++)
            {
                std::string create_table = "CREATE TABLE TestT"+ std::to_string(i) +" (A STRING, B INTEGER);";
                statements.push_back(create_table);
            }

            for(int i = 0; i < TestConstants::MaxTables; i++)
            {
                std::string drop = "TestT"+std::to_string(i);
                table_name.push_back(drop);
            }
            break;
        }
        
        case MODE_INSERT:
        {
            for(int i = 0; i < threadCount; i++)
            {
                std::string create_table = "CREATE TABLE TestT"+ std::to_string(i) +" (A STRING, B INTEGER);";
                statements.push_back(create_table);
            }

            for(int64_t i = 0; i < threadCount; i++)
            {
                test_data.push_back(i);
            }
        }
            break;
    }

    return *this;
}

// Last function that should be called when making a test, it will actually execute the test
// on the desired amount of threads based on the complexity.
TestResult DataStoreTest::test()
{ 
    //thread shared data initalize here
    DataStore data;
    share.data = &data;

    switch(mode)
    {
        case MODE_CREATE:
        {
            
            std::vector<std::thread> v;

            for (int i = 0; i < threadCount; i++)
            {
                i2tuple tuple = calculateArrayCut(threadCount, i);

                std::cout << std::get<0>(tuple) << " ";
                std::cout << std::get<1>(tuple) << "\n";
                std::vector<std::string> cut(&statements[std::get<0>(tuple)], &statements[std::get<1>(tuple)]);

                std::thread t1(createTest, cut, (void *) &share);
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
        
        case MODE_DROP:
        {

            std::vector<std::thread> v;
            std::vector<std::thread> v_t;

            //create table
            for (int i = 0; i < threadCount; ++i)
            {
                i2tuple tuple = calculateArrayCut(threadCount, i);
                std::vector<std::string> cut(&statements[std::get<0>(tuple)], &statements[std::get<1>(tuple)]);

                std::thread t1(createTest, cut, (void *) &share);
                v.push_back(std::move(t1));
            }

            for (int i = 0; i < threadCount; ++i)
            {
                v.at(i).join();
            }

            //drop table
            for (int i = 0; i < threadCount; ++i)
            {
                i2tuple tuple = calculateArrayCut(threadCount, i); 

                std::vector<std::string> cut(&table_name[std::get<0>(tuple)], &table_name[std::get<1>(tuple)]);
                
                std::thread t(dropTest, table_name, (void *) &share);
                v_t.push_back(std::move(t));
            }

            auto time_start = std::chrono::high_resolution_clock::now();

            for(int i = 0; i < threadCount; ++i)
            {
                v_t.at(i).join();
            }

            auto time_end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start);

            TestResult testResult(duration.count(), threadCount);
            return testResult;
            
        }
        case MODE_INSERT:
        {
            std::vector<std::thread> v;
            std::vector<std::thread> v_t;

            //create table
            for (int i = 0; i < threadCount; ++i)
            {
                std::thread t1(createTest, statements, (void *) &share);
                v.push_back(std::move(t1));
            }

            for (int i = 0; i < threadCount; ++i)
            {
                v.at(i).join();
            }

            auto time_start = std::chrono::high_resolution_clock::now();

            //insert data
            for(int i = 0; i < threadCount; i++)
            {
                std::thread t(insertTest, test_data, (void *) &share);
                v_t.push_back(std::move(t));
            }

            for(int i = 0; i < threadCount; i++)
            {
                v_t.at(i).join();
            }
            auto time_end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start);
        }
    }

    // Placeholder
    TestResult res(0ul, threadCount);
    return res;
}

i2tuple DataStoreTest::calculateArrayCut(int threadCount, int threadNumber)
{
    int size = statements.size();
    int cutPerThread = size / threadCount;
    int start = cutPerThread*threadNumber;
    int end = start + cutPerThread;

    return i2tuple(start, end);
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

DataStoreTest& DataStoreTest::setThreadCount(int count)
{
    threadCount = count;
    return *this;
}

