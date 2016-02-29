#include <stdio.h>
#include <string>
#include <vector>

#include "Test.h"
#include "Randomizer.h"
#include "Modes.h"
#include "SQLGenerator.h"
#include <sql/omdb_parser.h>



Test::Test(){}

Test& Test::with(int mode)
{
    this->mode = mode;
    
    return *this;
}

Test& Test::test()
{
    printf("Test mode: %d\n", mode);
    
    switch(mode)
    {
            
        case MODE_UNSET:
            printf("Select a mode! \n");
            break;
            
        case MODE_CREATE:
            
            printf("Start CREATE test: \n");
            
            //cout << statements[0] << endl;  
            
            break;
    }
    
    return *this;
}

Test& Test::generateCases(int complexity, int amount)
{
       

    SQLGenerator sqlGenerator;

    switch(mode)
    {
        case MODE_CREATE:
            printf("Generating CREATE test cases \n");
            
            statements = sqlGenerator.generateCreateCases(complexity, amount);
            printf("%d\n", statements.size());


            break;
                   
    }
    return *this;
}

void Test::clean()
{
    mode = -1;
    statements.clear();
}