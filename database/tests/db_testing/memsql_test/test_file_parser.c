//
// Created by mike on 4/8/16.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_file_parser.h"


void allocateCommandArraysAndTestString() {
    testName = (char*) malloc(MAX_COMMAND_LENGTH);
}

void deallocateCommandArraysAndTestString() {
    free(testName);
}


bool parseTestFile(FILE* fp) {
    // String to hold each command
    char* commandString = (char*) malloc(MAX_COMMAND_LENGTH);

    //First line is supposed to be test name
    testName = (char*) malloc(MAX_COMMAND_LENGTH);
    fgets(testName, MAX_COMMAND_LENGTH, fp);

    // Make sure that the first line specified the test to run
    if (strcmp(testName, "create_test\n") != 0 && strcmp(testName, "insert_test\n") != 0 &&
            strcmp(testName, "select_test\n")) {
        printf("Test type was not first line; recieved %s\n", testName);
        return false;
    }

    // Read the first command
    fgets(commandString, MAX_COMMAND_LENGTH, fp);
    // Read the file until the word end
    while (strcmp(commandString, TERMINATOR_STRING) != 0) {
        if (strcmp(commandString, "create\n") == 0) {

            int i = 0;
            const char createStatementPrefix[] = "CREATE";
            size_t crPrefLen = strlen(createStatementPrefix);

            // Reading create statements while statements start with CREATE
            while (strncmp(fgets(commandString, MAX_COMMAND_LENGTH, fp), createStatementPrefix, crPrefLen) == 0) {
                // Add command to create array
                // Remove trailing newline '\n' before adding to command list
                strtok(commandString, "\n");
                strcpy(createStrings[i], commandString);
                createStrings[i][strlen(commandString)] = '\0';
                i++;
            }

        } else if (strcmp(commandString, "insert\n") == 0) {

            int i = 0;
            const char insertStatementPrefix[] = "INSERT";
            size_t insPrefLen = strlen(insertStatementPrefix);

            // Read insert statements while lines begin with INSERT
            while (strncmp(fgets(commandString, MAX_COMMAND_LENGTH, fp), insertStatementPrefix, insPrefLen) == 0) {
                // Add command to  insert array
                // Remove trailing newline '\n' before adding to command list
                strtok(commandString, "\n");
                strncpy(insertStrings[i], commandString, strlen(commandString));
                insertStrings[i][strlen(commandString)] = '\0';
                i++;
            }

        } else if (strcmp(commandString, "select\n") == 0) {

            int i = 0;
            const char selectStatementPrefix[] = "SELECT";
            size_t selPrefLen = strlen(selectStatementPrefix);

            // Read select statements while lines begin with SELECT
            while (strncmp(fgets(commandString, MAX_COMMAND_LENGTH, fp), selectStatementPrefix, selPrefLen) == 0) {
                // Add command to select array
                // Remove trailing newline '\n' before adding to command list
                strtok(commandString, "\n");
                strncpy(selectStrings[i], commandString, strlen(commandString));
                selectStrings[i][strlen(commandString)] = '\0';
                i++;
            }

        } else if (strcmp(commandString, "end")) {
            printf("Parsing completed!\n");
            return true;
        }else {
            printf("Unrecognized command identifier: %s\n", commandString);
            return false;
        }
    }

    return true;
}
