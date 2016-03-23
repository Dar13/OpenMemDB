#ifndef GRAPHTEST_H
#define GRAPHTEST_H

#include <vector>


class GraphTest
{
	public:
		GraphTest(int mode, int threadCount);
		void createOrAppendOutputFile(int time, int threadCount);
		void createPerformanceTest(int threadCount);
		
	
};

#endif // TESTRESULT_H