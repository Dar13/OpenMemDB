#include <stdio.h>
#include <iostream>
#include <fstream>
#include "Test.h"
#include "DataStoreTest.h"
#include "Modes.h"
#include "TestResult.h"
#include "GraphTest.h"

int main(int argc, char* argv[])
{
    // DataStoreTest dataStoreTest;

    // TestResult result = dataStoreTest.with(MODE_CREATE)
    // 				                 .generateCases(0b1000)
    // 				                 .test();

    // printf("Duration: %lu\n", result.duration);
    // printf("Thread Count: %d\n", result.threadCount);


	if(strcmp(argv[1], "pct") == 0)
	{
		int threadCount;
		std::ofstream outputFile;

		sscanf(argv[2], "%d", &threadCount);

		GraphTest graphTest(MODE_CREATE, threadCount);
	}

	if(strcmp(argv[1], "pdt") == 0)
	{
		std::cout << argv[1] << std::endl;
	}
    

    // GraphTest graphTest;

	return 1;
}