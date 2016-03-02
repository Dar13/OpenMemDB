#ifndef DATASTORETEST_H
#define DATASTORETEST_H

#include <vector>
#include <string>
#include "TestResult.h"

class DataStoreTest
{


	public:
		DataStoreTest();
		DataStoreTest& with(int mode);
		TestResult test();
		DataStoreTest& generateCases(int complexity);


	private:
		
		int complexity, mode, threadCount;
		bool isRandomized = false;
		std::vector<std::string> statements;

		void parseComplexity(int complexity);
		static void createTest(std::vector<std::string> statements);
		static void dropTest(std::vector<std::string> statements);
		static void insertTest(std::vector<std::string> statements);
		std::vector<std::string> generateCases();


};


#endif // DATASTORETEST_H