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

std::string getInput() {
    std::string input;
    std::cout << "> ";
    std::getline(std::cin, input, '\n');

    return input;
}

void printErrorString(libomdb::ResultStatus err_code)
{
    using libomdb::ResultStatus;
    switch(err_code)
    {
        case ResultStatus::FAILURE_DB_UNKNOWN_STATE:
            std::cout << "Database is in an unknown state!" << std::endl;
            break;
        case ResultStatus::FAILURE_OUT_MEMORY:
            std::cout << "Database cannot allocate more memory!" << std::endl;
            break;
        case ResultStatus::FAILURE_SYNTAX:
            std::cout << "Syntax error in statement. Please try again." << std::endl;
            break;
        case ResultStatus::FAILURE:
        default:
            std::cout << "General unknown error when executing statement" << std::endl;
            break;
    }
}

libomdb::Connection doConnect(std::string command) {
    if (command.compare("connect") != 0) {
        std::cout << "Be sure to type connect to connect :-)" << std::endl;
        return libomdb::Connection::errorConnection();
    }
    // Connect to the database with the given instructions
    std::string ip;
    int port;
    bool valid_port = false;
    std::string database;

    std::cout << "What is the ip to connect to?" << std::endl;
    ip = getInput();
    std::cout << "What is the port?" << std::endl;
    while(!valid_port)
    {
        try {
            port = std::stoi(getInput());
        } catch (std::invalid_argument& inv_arg) {
            continue;
        } catch (std::out_of_range& out_range) {
            continue;
        }

        valid_port = true;
    }
    std::cout << "What is the database name?" << std::endl;
    database = getInput();

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

    if(result.status != libomdb::ResultStatus::SUCCESS) {
        printErrorString(result.status);
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
            std::cout << (result.getValue(i)>>7) << "\t| ";
        }
        std::cout << std::endl;
        printLine('-');
    }
}

void doCommand(libomdb::Connection connection, std::string command) {
    auto result = connection.executeCommand(command);
    if (result.status == libomdb::ResultStatus::SUCCESS) {
        std::cout << "Executed command '" << command << "' , affected " << result.numAffected << " rows:" << std::endl;
    } else {
        printErrorString(result.status);
    }
}

int main () {
    auto connection = libomdb::Connection::errorConnection();
    // Ask the user to connect to a database
    // What port and ip is the database running on
    std::string command;
    std::cout << "Welcome to OpenMemDB" << std::endl;
    std::cout << "To connect to a database type connect" << std::endl;
    command = getInput();

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

        command = getInput();
    }

    std::cout << "Thanks for using OpenMemDB" << std::endl;
    return 0;
}
