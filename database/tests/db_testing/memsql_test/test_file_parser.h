#ifndef OPENMEMDB_TEST_FILE_PARSER_H
#define OPENMEMDB_TEST_FILE_PARSER_H

#define MAX_COMMAND_LENGTH 200
#define MAX_COMMANDS 500000
#define TERMINATOR_STRING "end"

#include <stdbool.h>
#include <stdio.h>

enum TestType {
    CREATE,
    INSERT,
    SELECT
};

/**
 * The arrays holding the command strings
 */
char createStrings[MAX_COMMANDS][MAX_COMMAND_LENGTH];
char selectStrings[MAX_COMMANDS][MAX_COMMAND_LENGTH];
char insertStrings[MAX_COMMANDS][MAX_COMMAND_LENGTH];

char* testName;

/**
 * Allocates memory for the command arrays and test name
 */
void allocateCommandArraysAndTestString();

/**
 * De-allocates command array space
 */
void deallocateCommandArraysAndTestString();

/**
 * Parses the test file and creates the command arrays
 * 
 * @param fp The pointer to the test file
 * @return True if parsing worked properly, false otherwise
 */
bool parseTestFile(FILE* fp);


#endif //OPENMEMDB_TEST_FILE_PARSER_H
