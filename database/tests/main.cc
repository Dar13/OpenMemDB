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

	if(strcmp(argv[1], "sqltf") == 0) 
	{
		DataStoreTest test;
		
		if(strcmp(argv[2], "ct") == 0)
		{
			test.with(MODE_CREATE)
				.generateCases(0b0000)
				.printStatementsToFile();
		}
		if(strcmp(argv[2], "dt") == 0)
		{
			test.with(MODE_DROP)
				.generateCases(0b0000)
				.printStatementsToFile();			
		}
		if(strcmp(argv[2], "irt") == 0)
		{
			test.with(MODE_INSERT)
				.generateCases(0b0000)
				.printStatementsToFile();				
		}
		if(strcmp(argv[2], "st") == 0)
		{
			test.with(MODE_SELECT)
				.generateCases(0b0000)
				.printStatementsToFile();			
		}
		if(strcmp(argv[2], "ut") == 0)
		{
			
		}
	}


	// int threadCount = 1;
	// GraphTest graphTest(MODE_INSERT, threadCount);
    

    // GraphTest graphTest;

	return 1;
}