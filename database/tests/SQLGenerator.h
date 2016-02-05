#ifndef SQLGENERATOR_H
#define SQLGENERATOR_H

#include <vector>
#include <string>


class SQLGenerator
{
	public:

		SQLGenerator();

		// Generate create statements
	    std::vector<std::string> generateCreateCases(int complexity, int amount);

		// Generate one create statement
    	std::string generateCreateStatement(int complexity);

	    // Generate select statements
    	std::vector<std::string> generateSelectCases(int complexity, int amount);

	private:


    	// SELECT creation, just templates for now, we need to do create table, and insert tests first
    	std::string generateSelectStatement(int complexity);

	
};



#endif // SQLGENERATOR_H