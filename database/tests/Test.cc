#include <stdio.h>
#include <string>
#include <vector>

#include "Test.h"
#include "Randomizer.h"
#include "Modes.h"
#include "SQLGenerator.h"
#include <sql/omdb_parser.h>

int testMode = -1;
vector<string> mainStatements;

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
            
            //cout << mainStatements[0] << endl;  
            
            break;
    }
    
    return *this;
}

Test& Test::generateCases(int complexity, int amount)
{
       

    SQLGenerator sqlGenerator;

    switch(testMode)
    {
        case MODE_CREATE:
            printf("Generating CREATE test cases \n");
            
            mainStatements = sqlGenerator.generateCreateCases(complexity, amount);

            for(auto i = mainStatements.begin(); i  != mainStatements.end(); i++)
            {
                std::cout << *i << std::endl;
            }
            
            break;
                   
    }
    return *this;
}

void Test::clean()
{
    testMode = -1;
    mainStatements.clear();
}