#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include "Test.h"
#include "DataStoreTest.h"
#include "Modes.h"
#include "TestResult.h"
#include "GraphTest.h"

int main(int argc, char* argv[])
{

	if(strcmp(argv[1], "pct") == 0)
	{
		int threadCount;
		std::ofstream outputFile;

		sscanf(argv[2], "%d", &threadCount);

		GraphTest graphTest(MODE_CREATE, threadCount);
	}

	if(strcmp(argv[1], "pdt") == 0)
	{
		int threadCount;
		sscanf(argv[2], "%d", &threadCount);

		GraphTest graphTest(MODE_DROP, threadCount);
	}

	if(strcmp(argv[1], "pirt") == 0)
	{
		int threadCount;
		sscanf(argv[2], "%d", &threadCount);

		GraphTest graphTest(MODE_INSERT, threadCount);
	}

	if(strcmp(argv[1], "pst") == 0) 
	{
		int threadCount;
		sscanf(argv[2], "%d", &threadCount);

		GraphTest graphTest(MODE_SELECT, threadCount);
	}


	// int threadCount = 1;
	// GraphTest graphTest(MODE_INSERT, threadCount);
    

    // GraphTest graphTest;

	return 1;
}