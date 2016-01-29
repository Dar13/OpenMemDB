/*
The MIT License (MIT)
Copyright (c) 2015 University of Central Florida's Computer Software Engineering
Scalable & Secure Systems (CSE - S3) Lab
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef CONNECTION_H
#define CONNECTION_H

// Standard library includes
#include <cstdint>
#include <string>

// Project includes.
#include "resultset.h"


namespace libomdb{
  
  /** Represents result of database command */
  typedef struct CommandResult {
    bool isSuccess;
    int numAffected;
  } CommandResult;



  /**
   * Class that represents the 
   */  
  class ConnectionMetaData {
   public:
    
    /** Gets the name of the connected database */
    std::string getDbName();    

    /** Gets the validity of the database */
    bool isValid();

   private:
    
    /** The name of the connected database */
    std::string m_databaseName;

    /** Tells if connection is valid (connected properly) */
    bool m_isValid;   
  };  



  
  /**
   * Represents a connection to a specific database.
   */
  class Connection {
   public:

   /**
    * Connects to specified location and returns object representing
    * connection
    * @param hostname  The name of the host to connect to
    * @param port      The name of the port to connect to
    * @param db        The name of the database to connect to
    */
    static libomdb::Connection connect(std::string hostname, 
                                       uint16_t port, 
                                       std::string db);
    

   /**
    * Disconnects the connection object.
    * @param connection The connection object to terminate
    */
    static void disconnect(libomdb::Connection connection);


    
    /**
     * Performs requested action on database
     * @param command The comand to execute
     * @return a CommandResult struct with result information.
     */
    libomdb::CommandResult executeCommand(std::string command);


    /**
     * Gets requested query results from database
     * @param query The query to execute
     * @return A ResultSet representing the results of the query.
     */
    libmem::ResultSet executeQuery(std::string query);
    

    /**
     * Gets the meta data object that describes the connection
     * @return the metadata object that describes the connection.
     */
    libomdb::ConnectionMetaData getMetaData();

   private:

    /** The file desciptor used to communicate over */
    uint16_t m_socket_fd;

    /** The meta data object that describes the connection */
    libomdb::ConnectionMetaData m_metaData;    
  };  

}


/*************************************************************************
 * Stand alone functions.
 *************************************************************************/



#endif /* CONNECTION_H */
