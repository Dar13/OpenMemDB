#include <stdio.h>
#include "Test.h"


int main()
{
    
    Test test;
    
    int testMode = MODE_CREATE;
    
    test.with(testMode)
        .generateCases(0, 10)
        .test();
    
	return 1;
}