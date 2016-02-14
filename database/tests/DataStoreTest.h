#ifndef DATASTORETEST_H
#define DATASTORETEST_H

#include <vector>
#include <string>

static int testMode, complexity;

class DataStoreTest
{

	public:
		DataStoreTest();
		int createTest(int complexity);
		DataStoreTest& with(int mode);
		DataStoreTest& test();
		DataStoreTest& generateCases(int complexity);

	private:
		void parseComplexity(int complexity);
		std::vector<std::string> generateCases();


};


#endif // DATASTORETEST_H