#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "GraphTest.h"
#include "Test.h"
#include "DataStoreTest.h"
#include "Modes.h"
#include "TestResult.h"


GraphTest::GraphTest(int mode, int threadCount)
{

    switch(mode) 
    {

        case MODE_CREATE:
            createPerformanceTest(threadCount);
            break;
        case MODE_DROP:
            dropPerformanceTest(threadCount);
            break;
        case MODE_INSERT:
            insertPerformanceTest(threadCount);
            break;
        case MODE_SELECT:
            selectPerformanceTest(threadCount);
            break;
        case MODE_MIXED:
            mixedPerformanceTest(threadCount);
            break;
    }


}

// ombdt is a plaintext file that contains test data (used by the python script to generate the graphs)
// Current format:

/*
*   TestName
*   test case count
*   Thread1 time
*   Thread2 time
*   Thread3 time
*   Thread 4 time
*   ... etc
*/

void GraphTest::createOrAppendOutputFile(int mode, int time, int threadCount)
{
    std::ofstream outputFile;


    // first thread, make a new file
    if(threadCount == 1)
    {        
        outputFile.open("test_data.omdbt");
        if(outputFile.is_open())
        {

            switch(mode) 
            {
                case MODE_CREATE:
                    outputFile << "create_test\n";
                    break;
                case MODE_DROP:
                    outputFile << "drop_test\n";
                    break;
                case MODE_INSERT:
                    outputFile << "insert_test\n";
                    break;
                case MODE_SELECT:
                    outputFile << "select_test\n";
                    break;
                case MODE_MIXED:
                    outputFile << "mixed_test\n";
                    break;
            }
            outputFile << threadCount << "\n";
            outputFile << time << "\n";
            outputFile.close();
        }
    }
    else // any other thread count means we just append
    {
        outputFile.open("test_data.omdbt", std::ios::app);
        if(outputFile.is_open())
        {
            outputFile << time << "\n";
            outputFile.close();
        }
    }
}

void GraphTest::createPerformanceTest(int threadCount)
{
    DataStoreTest dataStoreTest;

    TestResult result = dataStoreTest.with(MODE_CREATE)
                                  .generateCases(0b0000)
                                  .setThreadCount(threadCount)
                                  .test();

    createOrAppendOutputFile(MODE_CREATE, result.duration, threadCount);

}

void GraphTest::dropPerformanceTest(int threadCount)
{
    DataStoreTest dataStoreTest;

    TestResult result = dataStoreTest.with(MODE_DROP)
                                  .generateCases(0b0000)
                                  .setThreadCount(threadCount)
                                  .test();

    createOrAppendOutputFile(MODE_DROP, result.duration, threadCount);

}

void GraphTest::insertPerformanceTest(int threadCount)
{
    DataStoreTest dataStoreTest;

    TestResult result = dataStoreTest.with(MODE_INSERT)
                                    .generateCases(0b0000)
                                    .setThreadCount(threadCount)
                                    .test();

    createOrAppendOutputFile(MODE_INSERT, result.duration, threadCount);
}

void GraphTest::selectPerformanceTest(int threadCount)
{
    DataStoreTest dataStoreTest;

    TestResult result = dataStoreTest.with(MODE_SELECT)
                                    .generateCases(0b0000)
                                    .setThreadCount(threadCount)
                                    .test();

    createOrAppendOutputFile(MODE_SELECT, result.duration, threadCount);
}

void GraphTest::mixedPerformanceTest(int threadCount)
{
    DataStoreTest dataStoreTest;

    TestResult result = dataStoreTest.with(MODE_MIXED)
                                    .generateCases(0b0000)
                                    .setThreadCount(threadCount)
                                    .test();

    createOrAppendOutputFile(MODE_SELECT, result.duration, threadCount);
}