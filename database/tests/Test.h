#ifndef TEST_H
#define TEST_H

#include <string>
#include <vector>


class Test 
{
   public:
    Test();
    
    Test& with(int testMode);
    Test& test(); // return TestResult
    Test& generateCases(int complexity, int amount);
    bool isRandomized;
	int mode, complexity, threadCount;
	std::vector<std::string> statements; 
     
   
   private:
    void clean();

   protected:


};

#endif // TEST_H 