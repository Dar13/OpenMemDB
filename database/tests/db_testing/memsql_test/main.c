//
// Created by mike on 4/8/16.
//

#include <string.h>

#include "test_file_parser.h"
#include "test_runner.h"

#define DISTINCT_THREAD_COUNT 4

const static unsigned int threads[4] = {1, 2, 4, 8};

int main() {
    FILE* fp = fopen("../../sql_statements.omdbt", "r");
    if (fp == NULL) {
        fprintf(stderr, "File not read correctly\n");
        return 1;
    }

    allocateCommandArraysAndTestString();

    if (!parseTestFile(fp)) {
        printf("Parsing failed, aborting...\n");
        exit(1);
    }

    printf("testName is %s\n", testName);

    if (strcmp(testName, "insert_test") == 0) {
        for (int i = 0; i < DISTINCT_THREAD_COUNT; ++i) {
            executeInsertTest(threads[i]);
        }
    } else if (strcmp(testName, "create_test") == 0) {
        for (int i = 0; i < DISTINCT_THREAD_COUNT; ++i) {
            executeCreateTest(threads[i]);
        }
    } else if (strcmp(testName, "select_test")) {
        for (int i = 0; i < DISTINCT_THREAD_COUNT; ++i) {
            executeSelectTest(threads[i]);
        }
    } else {
        printf("Unknown test name: %s\n", testName);
    }

    deallocateCommandArraysAndTestString();

    return 0;
}
