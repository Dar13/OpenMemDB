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

    // Creating a new instance because it fails the position < secondary_array_size_ otherwise
    // todo: reset tervel after each test so I dont have to do this

    // std::vector<int> timeVals;
    // DataStoreTest dataStoreTest;

    // TestResult result = dataStoreTest.with(MODE_CREATE)
    // 				                 .generateCases(0b0000)
    // 				                 .test();

    // timeVals.push_back(result.duration);

    // DataStoreTest dataStoreTest2;

    // result = dataStoreTest2.with(MODE_CREATE)
    //                       .generateCases(0b0010)
    //                       .test();

    // timeVals.push_back(result.duration);

    // DataStoreTest dataStoreTest3;

    // result = dataStoreTest3.with(MODE_CREATE)
    //                       .generateCases(0b0100)
    //                       .test();

    // timeVals.push_back(result.duration);

    // DataStoreTest dataStoreTest4;

    // result = dataStoreTest4.with(MODE_CREATE)
    //                       .generateCases(0b1000)
    //                       .test();

    // timeVals.push_back(result.duration);

    // DataStoreTest dataStoreTest5;

    // result = dataStoreTest5.with(MODE_CREATE)
    //                                  .generateCases(0b0000)
    //                                  .setThreadCount(16)
    //                                  .test();

    // timeVals.push_back(result.duration);

    // DataStoreTest dataStoreTest6;

    // result = dataStoreTest6.with(MODE_CREATE)
    //                                 .generateCases(0b0000)
    //                                 .setThreadCount(32)
    //                                 .test();

    // timeVals.push_back(result.duration);

    // DataStoreTest dataStoreTest7;

    // result = dataStoreTest7.with(MODE_CREATE)
    //                             .generateCases(0b0000)
    //                             .setThreadCount(64)
    //                             .test();

    // timeVals.push_back(result.duration);

    // TestResult result = dataStoreTest.with(MODE_DROP)
    //                                 .generateCases(0b0100)
    //                                 .test();
                               
    // timeVals.push_back(result.duration);

    createPerformanceTest(threadCount);

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

void GraphTest::createOrAppendOutputFile(int time, int threadCount)
{
    std::ofstream outputFile;


    // first thread, make a new file
    if(threadCount == 1)
    {        
        outputFile.open("test_data.omdbt");
        if(outputFile.is_open())
        {
            outputFile << "CreateTest\n";
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

    createOrAppendOutputFile(result.duration, threadCount);

}