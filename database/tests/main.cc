#include <stdio.h>
#include "Test.h"


int main()
{
    
    Test test;
    
    int testMode = MODE_CREATE;
    
    test.with(testMode)
        .generateCases()
        .test();
    
	return 1;
}