#ifndef TESTRESULT_H
#define TESTRESULT_H

class TestResult
{
	public:
		TestResult(unsigned long duration, int threadCount);
		int threadCount;
		unsigned long duration;

	private:

};


#endif // TESTRESULT_H
