#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include "Test.h"
#include "DataStoreTest.h"
#include "Modes.h"
#include "TestResult.h"
#include "GraphTest.h"

void printCommands();

int main(int argc, char* argv[])
{
	if(argv[1] && argv[2]) 
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
		
		if(strcmp(argv[1], "pmt") == 0) 
		{
			int threadCount;
			sscanf(argv[2], "%d", &threadCount);

			GraphTest graphTest(MODE_MIXED, threadCount);
		}

		if(strcmp(argv[1], "sqltf") == 0) 
		{
			DataStoreTest test;
			
			if(strcmp(argv[2], "ct") == 0)
			{

				if(argv[3] && strcmp(argv[3], "-c") == 0) 
				{
					test.with(MODE_CREATE)
						.generateCompatCases(0b0000)
						.printStatementsToFile();
				}
				else 
				{
					test.with(MODE_CREATE)
						.generateCases(0b0000)
						.printStatementsToFile();
				}
			}
			if(strcmp(argv[2], "dt") == 0)
			{

				if(argv[3] && strcmp(argv[3], "-c") == 0) 
				{
					test.with(MODE_DROP)
						.generateCompatCases(0b0000)
						.printStatementsToFile();	
				}
				else
				{
					test.with(MODE_DROP)
						.generateCases(0b0000)
						.printStatementsToFile();	
				}
			}
			if(strcmp(argv[2], "irt") == 0)
			{

				if(argv[3] && strcmp(argv[3], "-c") == 0) 
				{
					test.with(MODE_INSERT)
						.generateCompatCases(0b0000)
						.printStatementsToFile();		
				}
				else
				{			
					test.with(MODE_INSERT)
						.generateCases(0b0000)
						.printStatementsToFile();				
				}

			}
			if(strcmp(argv[2], "st") == 0)
			{


				if(argv[3] && strcmp(argv[3], "-c") == 0) 
				{
					test.with(MODE_SELECT)
						.generateCompatCases(0b0000)
						.printStatementsToFile();
				}
				else
				{
					test.with(MODE_SELECT)
						.generateCases(0b0000)
						.printStatementsToFile();			
				}

			}
			if(strcmp(argv[2], "mt") == 0)
			{

				if(argv[3] && strcmp(argv[3], "-c") == 0) 
				{
					test.with(MODE_MIXED)
						.generateCompatCases(0b0000)
						.printStatementsToFile();
				}
				else
				{
					test.with(MODE_MIXED)
						.generateCases(0b0000)
						.printStatementsToFile();			
				}

			}
		}
	}
	else
	{
		printf("Invalid argument, here is a list of commands: \n");
		printCommands();
	}

	return 1;
}

void printCommands()
{
	printf("Performance test commands \n\n");
	printf("\tCreate table test:\tpct <thread count>\n");
	printf("\tDrop table test:\tpdt <thread count>\n");
	printf("\tInsert row test:\tpirt <thread count>\n");
	printf("\tSelect test:\t\tpst <thread count>\n");

	printf("_______________________________________________________________\n\n");

	printf("Print generated sql statments to file\n\n");
	printf("\tsqltf <test>\n\n");
	printf("\tWhere <test> is:\n\n");
	printf("\t\tct\n\t\tdt\n\t\tirt\n\t\tst\n\n");
	printf("\tFor create, drop, insert, and select, respectively\n");
	printf("\n\tthe -c flag with any of the sqltf commands\n");
	printf("\twill print it in compatibility mode with other DBs\n");

	printf("_______________________________________________________________\n\n");


}