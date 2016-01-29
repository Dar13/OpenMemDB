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

    // Randomized string creation
    string generateString(const int len);

    // Randomized ints
    int generateInt(const int max);
    
    // CREATE TABLE test
    string generateCreateStatement(int complexity);
    vector<string> generateCreateCases(int complexity);

    // SELECT Test, just templates for now, we need to do create table, and insert tests first
    string generateSelectStatement(int complexity);
    vector<string> generateSelectCases(int complexity);


    void clean();

};

#endif // TEST_H 