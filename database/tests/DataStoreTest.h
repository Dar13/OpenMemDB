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
		DataStoreTest& generateCompatCases(int complexity);
		DataStoreTest& setThreadCount(int count);
		TestResult test();
		void printStatementsToFile();
		

	private:
		
        struct thread_data
        {
            //tervel test and datastore must be shared by all threads
            tervel::Tervel* tervel_test;

            //tervel::ThreadContext* main_context;
            DataStore *data;
        }share;
        

		int complexity, mode, threadCount;
		bool isRandomized = false;
		std::vector<std::string> statements;
        std::vector<std::string> test_data;
        std::vector<std::string> table_name;
        std::vector<std::string> select_data;
        std::vector<std::string> all_data;

		void parseComplexity(int complexity);
		static void createTest(std::vector<std::string> statements, void *t_data);
        static void dropTest(std::vector<std::string> table_name, void *t_data);
        static void insertTest(std::vector<std::string> record, void *t_data);
        static void selectTest(std::vector<std::string> select_statements, void *t_data);
        static void mixedTest(std::vector<std::string> all_statements, void *t_data);
		static tervel::ThreadContext* loadTables(std::vector<std::string> statements, void *t_data);
		static tervel::ThreadContext* loadRows(std::vector<std::string> records, 
			std::vector<std::string> statements, void *t_data);
		i2tuple calculateArrayCut(int threadCount, int threadNumber);

};


#endif // DATASTORETEST_H
