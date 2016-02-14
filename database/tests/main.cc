#include <stdio.h>
#include "Test.h"
#include "DataStoreTest.h"
#include "Modes.h"


int main()
{
    
    // Test test;
    
    // int testMode = MODE_CREATE;"CREATE TABLE TestTable (A INTEGER, B INTEGER);"
    
    // test.with(testMode)
    //     .generateCases(1, 100)
    //     .test();
    

    DataStoreTest dataStoreTest;

    dataStoreTest.with(MODE_CREATE)
    				.generateCases(0b0010)
    				.test();

	return 1;
}