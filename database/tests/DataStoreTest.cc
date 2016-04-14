#include <data/data_store.h>
#include <sql/omdb_parser.h>
#include <tervel/util/tervel.h>
#include <workmanager/work_manager.h>
#include <workmanager/work_thread.h>

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
#include <iostream>
#include <fstream>
#include <mutex>
#include <condition_variable>

std::mutex mu;
std::condition_variable cond;

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

    std::unique_lock<std::mutex> locker(mu);

    cond.wait(locker);
    locker.unlock();

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
                successCount++;

            }
        }
    }

    delete main_context;
}

tervel::ThreadContext* DataStoreTest::loadTables(std::vector<std::string> statements, void *t_data)
{
    struct thread_data *grab;
    grab = (struct thread_data *) t_data;
    int successCount = 0;

    setupTokenMappings();

    tervel::ThreadContext* main_context = new tervel::ThreadContext(grab->tervel_test);
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
                successCount++;

            }
        }
    }

    return main_context;
}

void DataStoreTest::dropTest(std::vector<std::string> table_name, void *t_data)
{
    struct thread_data *grab;
    grab = (struct thread_data *) t_data;

    setupTokenMappings();

    tervel::Tervel* tervel_test = grab->tervel_test;
    tervel::ThreadContext* main_context = new tervel::ThreadContext(tervel_test);

    DataStore *data = grab->data;

    std::unique_lock<std::mutex> locker(mu);
    
    cond.wait(locker);
    locker.unlock();

    //Drop tables after creating them
    for(auto i = table_name.begin(); i != table_name.end(); i++)
    {
        auto err = data->deleteTable(*i);
    }

    delete main_context;
}


void DataStoreTest::insertTest(std::vector<std::string> records, void *t_data)
{
    struct thread_data *grab;
    grab = (struct thread_data *) t_data;

    setupTokenMappings();

    tervel::Tervel* tervel_test = grab->tervel_test;
    tervel::ThreadContext* main_context = new tervel::ThreadContext(tervel_test);    
    DataStore *data = grab->data;

    std::unique_lock<std::mutex> locker(mu);

    cond.wait(locker);
    locker.unlock();

    for(auto i = records.begin(); i != records.end(); i++)
    {
        ParseResult parse_result = parse(*i, data);

        if(parse_result.status == ResultStatus::SUCCESS)
        {
            auto result = WorkThread::ExecuteStatement(parse_result.result, data);
            if(result.status == ResultStatus::SUCCESS) 
            {
            }
        }
    }

    delete main_context;
}

tervel::ThreadContext* DataStoreTest::loadRows(std::vector<std::string> records, 
    std::vector<std::string> statements, void *t_data)
{
    struct thread_data *grab;
    grab = (struct thread_data *) t_data;

    setupTokenMappings();

    DataStore *data = grab->data;

    tervel::ThreadContext* main_context = loadTables(statements, t_data);

    for(auto i = records.begin(); i != records.end(); i++)
    {
        ParseResult parse_result = parse(*i, data);

        if(parse_result.status == ResultStatus::SUCCESS)
        {
            auto result = WorkThread::ExecuteStatement(parse_result.result, data);
            if(result.status == ResultStatus::SUCCESS) 
            {
            }
        }
    }

    return main_context;
}


void DataStoreTest::selectTest(std::vector<std::string> select_statements, void *t_data)
{
    struct thread_data *grab;
    grab = (struct thread_data *) t_data;

    setupTokenMappings();   

    tervel::Tervel* tervel_test = grab->tervel_test;
    tervel::ThreadContext* main_context = new tervel::ThreadContext(tervel_test);    
    DataStore *data = grab->data;

    std::unique_lock<std::mutex> locker(mu);

    cond.wait(locker);
    locker.unlock();

    for(auto i = select_statements.begin(); i != select_statements.end(); i++)
    {
        std::cout << *i << std::endl;
        ParseResult parse_result = parse(*i, data);

        if(parse_result.status == ResultStatus::SUCCESS)
        {
            printf("we did it\n");

            auto result = WorkThread::ExecuteStatement(parse_result.result, data);
            if(result.status == ResultStatus::SUCCESS) 
            {
                printf("reddit\n");
            }
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
            for(int i = 0; i < TestConstants::InsertToTables; i++)
            {
                std::string create_table = "CREATE TABLE TestT"+ std::to_string(i) +" (A DATE, B INTEGER);";
                statements.push_back(create_table);
            }

            int year = 2016;
            
            for(int i = 0; i < TestConstants::MaxInserts; i++)
            {
                std::string date = std::to_string(year) + "-04-12";
                std::string insert_into = "INSERT INTO TestT0 VALUES (" + date + "," +
                std::to_string(i) + ");";
                
                test_data.push_back(insert_into);
            }
            break;
        }

        case MODE_SELECT:
        {

            for (int i = 0; i < TestConstants::SelectFromTables; ++i)
            {
                std::string create_table = "CREATE TABLE TestT" + std::to_string(i) + " (A DATE, B INTEGER);";
                statements.push_back(create_table);
            }

            int year = 2016;
            
            for(int i = 0; i < TestConstants::MaxSelects; ++i)
            {
                std::string date = std::to_string(year) + "-04-12";
                std::string insert_into = "INSERT INTO TestT0 VALUES (" + date + "," +
                std::to_string(i) + ");";

                test_data.push_back(insert_into);
            }

            for (int i = 0; i < TestConstants::MaxSelects; ++i)
            {
                // std::string select_from = "SELECT TestT0.B FROM TestT0 WHERE TestT0.B==" + std::to_string(i) + ";";
                std::string select_from = "SELECT TestT0.B FROM TestT0;";
                select_data.push_back(select_from);
            }

            break;
        }
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

                std::vector<std::string> cut(&statements[std::get<0>(tuple)], &statements[std::get<1>(tuple)]);
                std::thread t1(createTest, cut, (void *) &share);

                v.push_back(std::move(t1));
            }

            auto time_start = std::chrono::high_resolution_clock::now();

            cond.notify_all();

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

            std::vector<std::thread> v_t;

            tervel::ThreadContext* old_context = loadTables(statements, (void *) &share);
            
            //drop table
            for (int i = 0; i < threadCount; ++i)
            {
                i2tuple tuple = calculateArrayCut(threadCount, i); 

                std::vector<std::string> cut(&table_name[std::get<0>(tuple)], &table_name[std::get<1>(tuple)]);
                
                std::thread t(dropTest, cut, (void *) &share);
                v_t.push_back(std::move(t));
            }

            auto time_start = std::chrono::high_resolution_clock::now();
            
            cond.notify_all();

            for(int i = 0; i < threadCount; ++i)
            {
                v_t.at(i).join();
            }

            auto time_end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start);

            delete old_context;

            TestResult testResult(duration.count(), threadCount);
            return testResult;
            
        }
        case MODE_INSERT:
        {
            std::vector<std::thread> v_t;

            // Create the tables
            tervel::ThreadContext* old_context = loadTables(statements, (void *) &share);

            // insert threads
            for(int i = 0; i < threadCount; i++)
            {

                i2tuple tuple = calculateArrayCut(threadCount, i); 

                std::vector<std::string> cut(&test_data[std::get<0>(tuple)], &test_data[std::get<1>(tuple)]);
                std::thread t(insertTest, cut, (void *) &share);

                v_t.push_back(std::move(t));
            }

            auto time_start = std::chrono::high_resolution_clock::now();

            cond.notify_all();

            for(int i = 0; i < threadCount; i++)
            {
                v_t.at(i).join();
            }

            auto time_end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start);

            delete old_context;

            TestResult testResult(duration.count(), threadCount);
            return testResult;
        }
        case MODE_SELECT:
        {
            std::vector<std::thread> v_t;

            tervel::ThreadContext* old_context = loadRows(test_data, statements, (void *) &share);

            auto time_start = std::chrono::high_resolution_clock::now();

            for(int i = 0; i < threadCount; i++)
            {

                i2tuple tuple = calculateArrayCut(threadCount, i); 

                std::vector<std::string> cut(&select_data[std::get<0>(tuple)], &select_data[std::get<1>(tuple)]);
                std::thread t(selectTest, cut, (void *) &share);

                v_t.push_back(std::move(t));
            }

            cond.notify_all();

            for(int i = 0; i < threadCount; i++)
            {
                v_t.at(i).join();
            }

            auto time_end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start);
            
            delete old_context;
            break;
        }
    }

    // Placeholder
    TestResult res(0ul, threadCount);
    return res;
}

i2tuple DataStoreTest::calculateArrayCut(int threadCount, int threadNumber)
{
    int size;

    switch(mode) 
    {

        case MODE_CREATE:
        case MODE_DROP:
            size = statements.size();
            break;
        case MODE_INSERT:
            size = test_data.size();
            break;
        case MODE_SELECT:
            size = select_data.size();
        default:
            size = 0;
            break;
    }

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
    share.tervel_test = new tervel::Tervel(2*threadCount);
    return *this;
}

void DataStoreTest::printStatementsToFile()
{
    std::ofstream outputFile;
    outputFile.open("sql_statements.omdbt");

    switch(mode)
    {
        case MODE_CREATE:
        {
            outputFile << "create_test" << std::endl;
            outputFile << "create" << std::endl;

            for(auto i = statements.begin(); i  != statements.end(); i++)
            {
                outputFile << *i << std::endl;
            }

            break;
        }
        case MODE_DROP:
        {
            outputFile << "drop_test" << std::endl;
            outputFile << "create" << std::endl;

            for(auto i = statements.begin(); i  != statements.end(); i++)
            {
                outputFile << *i << std::endl;
            }    

            outputFile << "drop" << std::endl;

            for(auto i = table_name.begin(); i  != table_name.end(); i++)
            {
                outputFile << *i << std::endl;
            }     

            break;
        }
        case MODE_INSERT:
        {
            outputFile << "insert_test" << std::endl;
            outputFile << "create" << std::endl;

            for(auto i = statements.begin(); i  != statements.end(); i++)
            {
                outputFile << *i << std::endl;
            }

            outputFile << "insert" << std::endl;

            for(auto i = test_data.begin(); i  != test_data.end(); i++)
            {
                outputFile << *i << std::endl;
            }

            break;
        }
        case MODE_SELECT:
        {
            outputFile << "select_test" << std::endl;
            outputFile << "create" << std::endl;

            for(auto i = statements.begin(); i  != statements.end(); i++)
            {
                outputFile << *i << std::endl;
            }

            outputFile << "insert" << std::endl;

            for(auto i = test_data.begin(); i  != test_data.end(); i++)
            {
                outputFile << *i << std::endl;
            }

            outputFile << "select" << std::endl;

            for(auto i = select_data.begin(); i  != select_data.end(); i++)
            {
                outputFile << *i << std::endl;
            }

            break;
        }
    }
    
    outputFile << "end" << std::endl;
    outputFile.close();
}
