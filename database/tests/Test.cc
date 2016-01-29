#include "Test.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <random>

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

Test& Test::generateCases()
{
       
    switch(testMode)
    {
        case MODE_CREATE:
            printf("Generating CREATE test cases \n");
            
            generateCreateCases(1);

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
    // complexity is going to be a bitmask, still to be determined
    string base = "CREATE TABLE ";
    string ret = base;

    // Randomize name
    // Randomize length 
    int nameLength = generateInt(9) + 1;
    string name = generateString(nameLength);  

    // Add the name of the table to our result string
    ret += name;

    // Set up ret for the columns
    ret += " (";

    // Randomize column name
    nameLength = generateInt(4) + 1;
    string colName = generateString(nameLength);
    ret += colName;

    ret += " INTEGER, ";

    nameLength = generateInt(4) + 1;
    colName = generateString(nameLength);

    ret += colName;

    ret += " INTEGER);";

    return ret;
}

vector<string> Test::generateCreateCases(int complexity)
{

    // Todo: Check bitmask for complexity level, and 
    // create cases based on that

    int amount = generateInt(5) + 1;

    printf("Number of cases randomized to %d\n", amount);

    for(int i = 0; i < amount; i++)
    {
        string curStatement = generateCreateStatement(2);
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

string Test::generateString(const int len)
{
    char str[len];
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for(int i = 0; i < len; ++i)
    {
        str[i] = alphanum[generateInt((sizeof(alphanum) - 1))];
    }

    str[len] = 0;

    string ret(str);
    return ret;
}

int Test::generateInt(const int max)
{
    // Seed
    random_device rd;

    // Initialize generator
    mt19937 gen(rd());

    // Make a random uniform distribution
    uniform_int_distribution<> numbers(0,max);

    return numbers(gen);

}

void Test::clean()
{
    testMode = -1;
    statements.clear();
}