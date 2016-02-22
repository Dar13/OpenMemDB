#include "TestResult.h"

TestResult::TestResult(unsigned long duration, int threadCount)
{
	this->duration = duration;
	this->threadCount = threadCount;
}