//
// Created by mike on 3/22/16.
//

#include <iostream>
#include <cstring>
#include <algorithm>

#include "omdb_lib.h"

/**
 * Prints a line of 80 chars of passed in sybmol
 */
void printLine(char symbol) {
    for (int i = 0; i < 80; ++i) {
        std::cout << symbol;
    }
    std::cout<<std::endl;
}

void doConnect(libomdb::Connection* connection, std::string command) {
    if (command.compare("connect")) {
        std::cout << "Be sure to type connect to connect :-)" << std::endl;
        return;
    }
    // Connect to the database with the given instructions
    std::string ip;
    int port;
    std::string database;

    std::cout << "What is the ip to connect to?" << std::endl;
    std::cin >> ip;
    std::cout << "What is the port?" << std::endl;
    std::cin >> port;
    std::cout << "What is the database name?" << std::endl;
    std::cin >> database;

    try {
        *connection = libomdb::Connection::connect(ip, port, database);
        std::cout << "Successfully connected to " << database << std::endl;
    } catch (int e) {
        // I guess you catch ints in c++?
        std::cout << "Something went wrong\tException number " << e << std::endl;
    }
}

void doQuery(libomdb::Connection connection, std::string command) {
    auto result = connection.executeQuery(command);
    // Print all the results in a pretty little box
    // Start by printing the metadata columns
    auto metaData = result.getMetaData();
    printLine('-');
    std::cout << "| ";
    for (uint32_t i = 0; i < metaData.getColumnCount(); ++i) {
        std::cout << metaData.getColumnLabel(i) << "\t| ";
    }
    std::cout << std::endl;
    printLine('-');
    while (result.next()) {
        // Print one row per line
        std::cout << "| ";
        for (uint32_t i = 0; i < metaData.getColumnCount(); ++i) {
            std::cout << result.getValue(i) << "\t| ";
        }
        std::cout << std::endl;
        printLine('-');
    }
}

void doCommand(libomdb::Connection connection, std::string command) {
    auto result = connection.executeCommand(command);
    if (result.isSuccess) {
        std::cout << "Executed command " << command << ", affected " << result.numAffected << std::endl;
    } else {
        std::cout << "Something went wrong" << std::endl;
    }
}

int main () {
    libomdb::Connection *connection = NULL;
    // Ask the user to connect to a database
    // What port and ip is the database running on
    std::string command;
    std::cout << "Welcome to OMDB" << std::endl;
    std::cout << "To connect to a database type connect" << std::endl;
    std::cin >> command;
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    while(command.compare("exit") != 0) {
        if (command.empty()) {
            continue;
        }
        char firstLetter = command.at(0);
        if (firstLetter == 's') {
            if (connection == NULL) {
                std::cout << "You must first connect to a database" << std::endl;
            } else {
                doQuery(*connection, command);
            }
        } else {
            if (connection == NULL) {
                // Try to connect to the database
                doConnect(connection, command);
            } else {
                doCommand(*connection, command);
            }
        }

        std::cin >> command;
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    }

    std::cout << "Thanks for using OMDB" << std::endl;
    return 0;
}