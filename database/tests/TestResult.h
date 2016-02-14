#ifndef TESTRESULT_H
#define TESTRESULT_H

class TestResult
{

	int threadCount, executionTime;

	public:
		TestResult();

		int getExecutionTime();
		int getThreadCount();

	private:

};


#endif // TESTRESULT_H