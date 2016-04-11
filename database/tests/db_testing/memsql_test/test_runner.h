//
// Created by mike on 4/10/16.
//

#ifndef OPENMEMDB_TEST_RUNNER_H
#define OPENMEMDB_TEST_RUNNER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <time.h>
#include <pthread.h>

#define HOST "localhost"
#define USER "root"
#define PWD ""
#define PORT 3306

char** createStatements;
char** insertStatements;
char** selectStatements;

unsigned int NUM_OPERATIONS;

typedef struct worker_args {
    size_t id;
    unsigned int startIndex;
    unsigned int numThreads;
} worker_args;

void setStatements(char** creates, char** inserts, char** selects);

void setNumOps(unsigned int numOps);

bool connectToMemSql(MYSQL* conn, const char* host, const char* user, const char* pwd, const size_t port);

bool createDatabase(MYSQL* conn);

bool dropDatabase(MYSQL* conn);

void* createWorker(void* threadArgs);

void* insertWorker(void* threadArgs);

void* selectWorker(void* threadArgs);

clock_t executeCreateTest(unsigned int numThreads);

clock_t executeInsertTest(unsigned int numThreads);

clock_t executeSelectTest(unsigned int numThreads);

#endif //OPENMEMDB_TEST_RUNNER_H
