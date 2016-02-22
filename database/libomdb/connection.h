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
#include "result.h"
#include "../include/util/libomdb.h"
#include "serialization_helper.h"

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

    /** Builds new ConnectionMetaData object */
    ConnectionMetaData(std::string dbName, bool isValid);
    
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
     * Builds connection object
     * @param socket_fd The file desciptor of the socket to communicate over
     * @param serializedPacket The serialized packer returned from the server that describes the connection
     * @return A Connection object representing the passed in values.
     */
    static Connection buildConnectionObj(uint16_t socket_fd, char* serializedPacket);

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
    void disconnect();


    
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
    libomdb::Result executeQuery(std::string query);
    

    /**
     * Gets the meta data object that describes the connection
     * @return the metadata object that describes the connection.
     */
    libomdb::ConnectionMetaData getMetaData();

    /**
     * Sets the meta-data to the passed in object
     * @param data The ConnectionMetaData object for the related Connection object
     */
    void setMetaData(ConnectionMetaData data);

    /**
     * Creates a connection object that is invalid used on errors
     * @return An invalid connection object
     *
     * // Cannot figure out how to not do this in the .h Be my guest to fix. C++ is annoying.
     */
    static Connection errorConnection() {
      return Connection(0, ConnectionMetaData(NULL, false));
    }

   private:

    /**
     * Private constructor to create object
     * @param socket_fd The file descriptor used to communicate with the object
     * @param metaData  The ConnectionMetaData object describing the Connection.
     */
    Connection(uint64_t socket_fd, ConnectionMetaData metaData);

    /** The meta data object that describes the connection */
    libomdb::ConnectionMetaData m_metaData;

    /** The file descriptor used to communicate over */
    uint16_t m_socket_fd;

  };

}


/*************************************************************************
 * Stand alone functions.
 *************************************************************************/



#endif /* CONNECTION_H */
