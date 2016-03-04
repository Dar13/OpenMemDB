#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "GraphTest.h"
#include "Test.h"
#include "DataStoreTest.h"
#include "Modes.h"
#include "TestResult.h"


GraphTest::GraphTest()
{

    // Creating a new instance because it fails the position < secondary_array_size_ otherwise
    // todo: reset tervel after each test so I dont have to do this

    std::vector<int> timeVals;
    DataStoreTest dataStoreTest;

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

    // result = dataStoreTest.with(MODE_CREATE)
    //                                  .generateCases(0b0000)
    //                                  .setThreadCount(16)
    //                                  .test();

    // timeVals.push_back(result.duration);

    // DataStoreTest dataStoreTest6;

    // result = dataStoreTest.with(MODE_CREATE)
    //                                 .generateCases(0b0000)
    //                                 .setThreadCount(32)
    //                                 .test();

    // timeVals.push_back(result.duration);

    // DataStoreTest dataStoreTest7;

    TestResult result = dataStoreTest.with(MODE_CREATE)
                                    .generateCases(0b0000)
                                    .setThreadCount(48)
                                    .test();
                               
    timeVals.push_back(result.duration);

    createOutputFile(timeVals);
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
void GraphTest::createOutputFile(std::vector<int> timeVals)
{
    std::ofstream outputFile;
    outputFile.open("test_data.omdbt");
    outputFile << "CreateTest\n";
    outputFile << "5\n";

    for(auto const& testTime : timeVals)
    {
        outputFile << std::to_string(testTime) + "\n";
    }

    outputFile << "end \n";
}