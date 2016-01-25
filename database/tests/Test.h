#ifndef TEST_H
#define TEST_H

#include <string>
#include <vector>


using namespace std;

enum TestType 
{
    MODE_UNSET = -1,
    MODE_CREATE = 0,
    MODE_SELECT
};

class Test 
{

   public:
    Test();
    
    Test& with(int testMode);
    Test& test(); // return TestResult
    Test& generateCases();
   
   private:
    string generateCreateStatement();
    vector<string> generateCreateCases();
    void clean();

};

#endif // TEST_H 