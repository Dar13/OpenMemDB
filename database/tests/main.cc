#include <stdio.h>
#include "Test.h"
#include "DataStoreTest.h"


int main()
{
    
    Test test;
    
    int testMode = MODE_CREATE;
    
    // test.with(testMode)
    //     .generateCases(1, 100)
    //     .test();
    

    DataStoreTest dataStoreTest;

    dataStoreTest.createTest();

	return 1;
}