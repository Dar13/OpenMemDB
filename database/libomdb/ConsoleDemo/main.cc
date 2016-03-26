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

libomdb::Connection doConnect(std::string command) {
    if (command.compare("connect") != 0) {
        std::cout << "Be sure to type connect to connect :-)" << std::endl;
        return libomdb::Connection::errorConnection();
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

    printf("Connecting with (%s, %d, %s)\n", ip.c_str(), port, database.c_str());

    auto connection = libomdb::Connection::connect(ip, port, database);
    if (connection.getMetaData().isValid()) {
        std::cout << "Successfully connected to " << database << std::endl;
    } else {
        std::cout << "Connection unsuccessful" << std::endl;
    }
    return connection;
}

void doQuery(libomdb::Connection connection, std::string command) {
    std::cout << "Sending '" << command << "' to server" << std::endl;
    auto result = connection.executeQuery(command);

    if(!result.isValid) {
        // TODO: Print out error string based on error code returned
        printf("Error! I should be printing useful things...\n");
        return;
    }

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
        std::cout << "Executed command '" << command << "' , affected " << result.numAffected << " rows:" << std::endl;
    } else {
        // TODO: Print out error string based on error code returned
        printf("Error! I should be printing useful things...\n");
    }
}

int main () {
    auto connection = libomdb::Connection::errorConnection();
    // Ask the user to connect to a database
    // What port and ip is the database running on
    std::string command;
    std::cout << "Welcome to OMDB" << std::endl;
    std::cout << "To connect to a database type connect" << std::endl;
    std::getline(std::cin, command, '\n');

    while(command.compare("exit") != 0) {
        if (!command.empty()) {
            char firstLetter = command.at(0);
            if (firstLetter == 's' || firstLetter == 'S') {
                if (!connection.getMetaData().isValid()) {
                    std::cout << "You must first connect to a database" << std::endl;
                } else {
                    doQuery(connection, command);
                }
            } else {
                if (!connection.getMetaData().isValid()) {
                    // Try to connect to the database
                    connection = doConnect(command);
                } else {
                    doCommand(connection, command);
                }
            }
        }

        std::getline(std::cin, command, '\n');
    }

    std::cout << "Thanks for using OMDB" << std::endl;
    return 0;
}
