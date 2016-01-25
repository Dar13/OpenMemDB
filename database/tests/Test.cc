#include "Test.h"
#include <stdio.h>
#include <string>
#include <vector>

#include <sql/omdb_parser.h>
#include <data/data_store.h>

using namespace std;

int testMode = -1;

Test::Test(){}

Test& Test::with(int mode)
{
    testMode = mode;
    
    return *this;
}

Test& Test::test()
{
    printf("Test mode: %d\n", testMode);
    
    switch(testMode)
    {
            
        case MODE_UNSET:
            printf("Select a mode! \n");
            break;
            
        case MODE_CREATE:
            
            printf("Start CREATE test: \n");
            vector<string> statements = generateCases();
            
            cout << statements[0] << endl;  
            
            break;
    }
    
    return *this;
}

Test& Test::generateCases()
{
    
    switch(testMode)
    {
        case MODE_CREATE:
            printf("Generating CREATE test cases... \n");
            vector<string> statements = generateCreateCases();
            
            
            break;
                   
    }
    return *this;
}

string Test::generateCreateStatement()
{
    return "CREATE TABLE TestTable (A INTEGER, B INTEGER);";
}

vector<string> Test::generateCreateCases()
{
    vector<string> statements;
    string curStatement = generateCreateStatement();
    statements.push_back(curStatement);
    return statements;
}

void Test::clean()
{
    testMode = -1;
}