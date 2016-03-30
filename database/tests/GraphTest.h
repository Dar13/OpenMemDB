#ifndef GRAPHTEST_H
#define GRAPHTEST_H

#include <vector>


class GraphTest
{
	public:
		GraphTest(int mode, int threadCount);
		void createOrAppendOutputFile(int mode, int time, int threadCount);
		void createPerformanceTest(int threadCount);
		void dropPerformanceTest(int threadCount);
		void insertPerformanceTest(int threadCount);
		void selectPerformanceTest(int threadCount);

	
};

#endif // TESTRESULT_H