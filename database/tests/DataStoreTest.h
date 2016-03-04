#ifndef DATASTORETEST_H
#define DATASTORETEST_H

#include <tervel/util/tervel.h>
#include <data/data_store.h>

#include <vector>
#include <string>
#include <atomic>
#include <tuple>
#include "TestResult.h"

typedef std::tuple<int,int> i2tuple;

class DataStoreTest
{


	public:
		DataStoreTest();
		DataStoreTest& with(int mode);
		DataStoreTest& generateCases(int complexity);
		DataStoreTest& setThreadCount(int count);
		TestResult test();

	private:
		
		int complexity, mode, threadCount;
		bool isRandomized = false;
		std::vector<std::string> statements;

		void parseComplexity(int complexity);
		static void createTest(std::vector<std::string> statements, DataStore *data);
		static void dropTest(std::vector<std::string> statements, DataStore *data);
		static void insertTest(std::vector<std::string> statements);
		std::vector<std::string> generateCases();
		i2tuple calculateArrayCut(int threadCount, int threadNumber);

};


#endif // DATASTORETEST_H