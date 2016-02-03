#include "Test.h"
#include <stdio.h>
#include <string>
#include <vector>
#include "Randomizer.h"

#include <sql/omdb_parser.h>
#include <data/data_store.h>

using namespace std;


int testMode = -1;
vector<string> statements;

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
            
            //cout << statements[0] << endl;  
            
            break;
    }
    
    return *this;
}

Test& Test::generateCases(int complexity, int amount)
{
       
    switch(testMode)
    {
        case MODE_CREATE:
            printf("Generating CREATE test cases \n");
            
            generateCreateCases(complexity, amount);

            for(auto i = statements.begin(); i  != statements.end(); i++)
            {
                cout << *i << endl;
            }
            
            break;
                   
    }
    return *this;
}


// TODO: use complexity bitmask for the generation 
// Plan so far: mutation factor (number of columns)
// integer mutation
string Test::generateCreateStatement(int complexity)
{
    // complexity is a bitmask, the last digit represents column number randomization
    string base = "CREATE TABLE ";
    string ret = base;

    // Randomize name
    // Randomize length 
    int nameLength = Randomizer::generateInt(9) + 1;
    string name = Randomizer::generateString(nameLength);

    // Add the name of the table to our result string
    ret += name;

    // Set up ret for the columns
    ret += " (";

    // Bit mask, the last bit represents randomizing amount of columns.
    int columnAmountRandomization = complexity & 00001;

    if(columnAmountRandomization == 1)
    {

        int colAmount = Randomizer::generateInt(4) + 2;

        // Randomize all the columns except the last one
        for(int i = 0; i < colAmount-1; i++)
        {
            // Randomize column name
            nameLength = Randomizer::generateInt(4) + 1;
            string colName = Randomizer::generateString(nameLength);
            ret += colName;
            ret += " INTEGER, ";

        }

        // Fill in the end of the statement
        nameLength = Randomizer::generateInt(4) + 1;
        string colName = Randomizer::generateString(nameLength);

        ret += colName;

        ret += " INTEGER);";

        return ret;

    }
    else 
    {
        nameLength = Randomizer::generateInt(4) + 1;
        string colName = Randomizer::generateString(nameLength);
        ret += colName;

        ret += " INTEGER, ";

        nameLength = Randomizer::generateInt(4) + 1;
        colName = Randomizer::generateString(nameLength);

        ret += colName;

        ret += " INTEGER);";

        return ret;
    }
}

vector<string> Test::generateCreateCases(int complexity, int amount)
{

    // Todo: Check bitmask for complexity level, and 
    // create cases based on that

    for(int i = 0; i < amount; i++)
    {
        string curStatement = generateCreateStatement(complexity);
        statements.push_back(curStatement);
    }

    return statements;
}

string Test::generateSelectStatement(int complexity)
{

    return "SELECT *";
}


vector<string> Test::generateSelectCases(int complexity)
{

    vector<string> statements;
    return statements;
}

void Test::clean()
{
    testMode = -1;
    statements.clear();
}