#include <stdio.h>
#include <string>
#include <vector>

#include "Randomizer.h"
#include "SQLGenerator.h"

SQLGenerator::SQLGenerator(){}

vector<string> SQLGenerator::generateCreateCases(int complexity, int amount)
{
	std::vector<string> statements;
    // Todo: Check bitmask for complexity level, and 
    // create cases based on that

    for(int i = 0; i < amount; i++)
    {
        std::string curStatement = generateCreateStatement(complexity);
        statements.push_back(curStatement);
    }

    return statements;
}


// TODO: use complexity bitmask for the generation 
// Plan so far: mutation factor (number of columns)
// integer mutation

// TODO: fix occasions where it makes a name with no characters
string SQLGenerator::generateCreateStatement(int complexity)
{
    // complexity is a bitmask, the last digit represents column number randomization
    std::string base = "CREATE TABLE ";
    std::string ret = base;

    // Randomize name and length 
    int nameLength = Randomizer::generateInt(9) + 1;
    std::string name = Randomizer::generateString(nameLength);

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
            std::string colName = Randomizer::generateString(nameLength);
            ret += colName;
            ret += " INTEGER, ";

        }

        // Fill in the end of the statement
        nameLength = Randomizer::generateInt(4) + 1;
        std::string colName = Randomizer::generateString(nameLength);

        ret += colName;

        ret += " INTEGER);";

        return ret;

    }
    else 
    {
        nameLength = Randomizer::generateInt(4) + 1;
       	std::string colName = Randomizer::generateString(nameLength);
        ret += colName;

        ret += " INTEGER, ";

        nameLength = Randomizer::generateInt(4) + 1;
        colName = Randomizer::generateString(nameLength);

        ret += colName;

        ret += " INTEGER);";

        return ret;
    }
}

vector<string> SQLGenerator::generateSelectCases(int complexity, int amount)
{

    std::vector<string> selectStatements;
    return selectStatements;
}

string SQLGenerator::generateSelectStatement(int complexity)
{

    return "SELECT *";
}

