#include <stdio.h>
#include "Test.h"
#include "DataStoreTest.h"
#include "Modes.h"
#include "TestResult.h"


int main()
{
    
    // Test test;
    
    // int testMode = MODE_CREATE;"CREATE TABLE TestTable (A INTEGER, B INTEGER);"
    
    // test.with(testMode)
    //     .generateCases(1, 100)
    //     .test();
    

    DataStoreTest dataStoreTest;

    TestResult result = dataStoreTest.with(MODE_CREATE)
    				                 .generateCases(0b1000)
    				                 .test();

    printf("Duration: %lu\n", result.duration);
    printf("Thread Count: %d\n", result.threadCount);

	return 1;
}