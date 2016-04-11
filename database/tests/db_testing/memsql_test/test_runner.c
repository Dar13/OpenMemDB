//
// Created by mike on 4/10/16.
//

#include <stdint.h>

#include "test_runner.h"

void setNumOps(unsigned int numOps) {
    NUM_OPERATIONS = numOps;
}


void setStatements(char** creates, char** inserts, char** selects) {
    createStatements = creates;
    insertStatements = inserts;
    selectStatements = selects;
}


bool connectToMemSql(MYSQL* conn, const char* host, const char* user, const char* pwd, const size_t port) {
    printf("Connection to MemSQL... \n");
    if (mysql_real_connect(conn, host, user, pwd, NULL, port, NULL, 0) != conn) {
        printf("Could not connect to the MemSQL database!\n");
        return false;
    }
    return true;
}


bool createDatabase(MYSQL* conn) {
    printf("Creating database test...\n");
    if (mysql_query(conn, "create database test") || mysql_query(conn, "use test")) {
        printf("Could not create 'test' database!\n");
        return false;
    }
    return true;
}


bool dropDatabase(MYSQL* conn) {
    if (mysql_query(conn, "drop database test")) {
        printf("Could not drop the testing database 'test'!\n");
        return false;
    }
    mysql_close(conn);
    return true;
}


clock_t executeCreateTest(unsigned int numThreads) {
    // Set up mysql connector
    MYSQL conn;
    mysql_init(&conn);

    // Connect to MemSQL
    if (!connectToMemSql(&conn, HOST, USER, PWD, PORT)) {
        exit(1);
    }

    // Create database
    if (!createDatabase(&conn)) {
        exit(1);
    }

    // Create the threads
    // TODO: Wait and launch all threads at the same time
    pthread_t workers[numThreads];
    int numToWork = NUM_OPERATIONS/numThreads;
    // Start timer
    clock_t start = clock();
    for (int i = 0; i < numThreads; ++i) {
        worker_args args = {.id = i, .startIndex = numToWork*i, .numThreads = numThreads};
        pthread_create(&workers[i], NULL, &createWorker, &args);
    }

    for (int i = 0; i < numThreads; ++i) {
        size_t rows_i = 0;
        pthread_join(workers[i], &rows_i);
    }

    // Stop timer
    clock_t end = clock();

    printf("Cleaning up\n");
    // Drop the database
    dropDatabase(&conn);

    return end - start;
}


clock_t executeInsertTest(unsigned int numThreads) {
    // Set up mysql connector
    MYSQL conn;
    mysql_init(&conn);

    // Connect to MemSQL
    if (!connectToMemSql(&conn, HOST, USER, PWD, PORT)) {
        exit(1);
    }

    // Create database
    if (!createDatabase(&conn)) {
        exit(1);
    }

    // Create the table that the inserts will take place on
    if (mysql_query(&conn, createStatements[0])) {
        printf("Could not create test table for inserts\n");
        return clock() - clock();
    }

    // Do the inserts
    // Create the threads
    // TODO: Wait and launch all threads at the same time
    pthread_t workers[numThreads];
    int numToWork = NUM_OPERATIONS/numThreads;
    // Start timer
    clock_t start = clock();
    for (int i = 0; i < numThreads; ++i) {
        worker_args args = {.id = i, .startIndex = numToWork*i, .numThreads = numThreads};
        pthread_create(&workers[i], NULL, &insertWorker, &args);
    }

    for (int i = 0; i < numThreads; ++i) {
        size_t rows_i = 0;
        pthread_join(workers[i], &rows_i);
    }

    // Stop timer
    clock_t end = clock();

    printf("Cleaning up\n");
    // Drop the database
    dropDatabase(&conn);

    return end - start;

}


clock_t executeSelectTest(unsigned int numThreads) {
    // Set up mysql connector
    MYSQL conn;
    mysql_init(&conn);

    // Connect to MemSQL
    if (!connectToMemSql(&conn, HOST, USER, PWD, PORT)) {
        exit(1);
    }

    // Create database
    if (!createDatabase(&conn)) {
        exit(1);
    }

    // Create the table to test on
    if (mysql_query(&conn, createStatements[0])) {
        printf("Could not create test table for select\n");
        return clock() - clock();
    }


    // Do the inserts to set up the table
    for (int i = 0; i < (sizeof(insertStatements)/ sizeof(insertStatements[0])); ++i) {
        if (mysql_query(&conn, insertStatements[i])) {
            printf("Could not execute an insert statement for select test");
            return clock() - clock();
        }
    }


    // Create the threads
    // TODO: Wait and launch all threads at the same time
    pthread_t workers[numThreads];
    // Start timer
    int numToWork = NUM_OPERATIONS/numThreads;
    clock_t start = clock();
    for (int i = 0; i < numThreads; ++i) {
        worker_args args = {.id = i, .startIndex = numToWork*i, .numThreads = numThreads};
        pthread_create(&workers[i], NULL, &selectWorker, &args);
    }

    for (int i = 0; i < numThreads; ++i) {
        size_t rows_i = 0;
        pthread_join(workers[i], &rows_i);
    }

    // Stop timer
    clock_t end = clock();

    printf("Cleaning up\n");
    // Drop the database
    dropDatabase(&conn);

    return end - start;

}


void* createWorker(void* threadArgs) {
    worker_args* args = (worker_args*)threadArgs;
    // connect to the created db
    MYSQL conn;
    mysql_init(&conn);
    if (mysql_real_connect(&conn, HOST, USER, PWD, "test", PORT, NULL, 0) != &conn) {
        printf("Worker %zu could not connect to the MemSQL database! Aborting...\n", args->id);
        exit(1);
    }

    // Insert the number of operations / number of threads amount
    for (int i = 0; i < NUM_OPERATIONS/args->numThreads; ++i) {
        if (mysql_query(&conn, createStatements[i+args->startIndex])) {
            printf("Worker %zu failed to insert data, aborting...\n", args->id);
            exit(1);
        }
    }

    mysql_close(&conn);
    uintptr_t numRows = (NUM_OPERATIONS/args->numThreads);
    return (void*)numRows;
}


void* insertWorker(void* threadArgs) {
    worker_args* args = (worker_args*)threadArgs;
    // connect to the created db
    MYSQL conn;
    mysql_init(&conn);
    if (mysql_real_connect(&conn, HOST, USER, PWD, "test", PORT, NULL, 0) != &conn) {
        printf("Worker %zu could not connect to the MemSQL database! Aborting...\n", args->id);
        exit(1);
    }

    // Insert the number of operations / number of threads amount
    for (int i = 0; i < NUM_OPERATIONS/args->numThreads; ++i) {
        if (mysql_query(&conn, insertStatements[i+args->startIndex])) {
            printf("Worker %zu failed to insert data, aborting...\n", args->id);
            exit(1);
        }
    }

    mysql_close(&conn);

    uintptr_t numRows = (NUM_OPERATIONS/args->numThreads);
    return (void*)numRows;
}


void* selectWorker(void* threadArgs) {
    worker_args* args = (worker_args*)threadArgs;
    // connect to the created db
    MYSQL conn;
    mysql_init(&conn);
    if (mysql_real_connect(&conn, HOST, USER, PWD, "test", PORT, NULL, 0) != &conn) {
        printf("Worker %zu could not connect to the MemSQL database! Aborting...\n", args->id);
        exit(1);
    }

    // Insert the number of operations / number of threads amount
    for (int i = 0; i < NUM_OPERATIONS/args->numThreads; ++i) {
        if (mysql_query(&conn, selectStatements[i+args->startIndex])) {
            printf("Worker %zu failed to insert data, aborting...\n", args->id);
            exit(1);
        }
    }

    mysql_close(&conn);
    uintptr_t numRows = (NUM_OPERATIONS/args->numThreads);
    return (void*)numRows;

}
