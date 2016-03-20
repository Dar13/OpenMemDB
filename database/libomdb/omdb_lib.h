//
// Created by mike on 2/22/16.
//

#ifndef OPENMEMDB_LIBOMDB_H
#define OPENMEMDB_LIBOMDB_H

// Standard library includes
#include <cstdint>
#include <string>
#include <vector>

#define MESSAGE_SIZE 300

namespace libomdb
{
  const uint8_t THE_TERMINATOR = 0xFF;
  const uint32_t DB_NAME_LEN = 50;
  const uint32_t COL_NAME_LEN = 25;
  const uint32_t MAX_NUM_COLUMNS = 20;
  const uint32_t MAX_PACKET_SIZE = 512;

  enum class ResultStatus : uint16_t
  {
    SUCCESS = 0,
    FAILURE,
    FAILURE_OUT_MEMORY,
  };
  
  enum class ManipStatus : uint32_t
  {
    SUCCESS = 0,
    ERR,
    ERR_NO_MEMORY,
    ERR_TABLE_NOT_EXIST,
    ERR_TABLE_CMD_INVALID,
    ERR_CONTENTION,
    ERR_PARTIAL,
    ERR_PARTIAL_CONTENTION,
  };
  
  /**
   * Enum used to describe the type of statement being sent
   */
  enum class CommandType: uint8_t {
    SQL_STATEMENT = 0x01,
    DB_COMMAND = 0x02,
    INVALID_COMMAND
  };
  
  /**
   * Enum used to describe the type of packet being sent
   */
  enum class PacketType: uint8_t {
    NONE = 0,
    COMMAND,
    COMMAND_RESULT,
    CONNECTION,
    RESULT_METADATA,
    RESULT_DATA,
    INVALID_PACKET
  };
  
  /** Represents result of database command */
  struct CommandResult {
      bool isSuccess;
      int numAffected;
  };
  
  struct CommandPacket {
      PacketType type;
    CommandType commandType;  // The type of command requested to be executed: SQL_STATEMENT or DB_COMMAND
    char message[MESSAGE_SIZE];
    uint8_t terminator;
  }__attribute__((packed));
  
  struct CommandResultPacket {
      PacketType type;
      ResultStatus status;
      ManipStatus secondaryStatus;
      uint32_t rowsAffected;
      uint8_t terminator;
  } __attribute__((packed));
  
  struct ConnectionPacket {
    PacketType type;
    /** The name of the database */
    char name[DB_NAME_LEN]; // The name of the database connected to.
  }__attribute__((packed));
  
  
  struct ResultColumn {
    char name[COL_NAME_LEN];  // The column name
    uint16_t type;            // The SQL type of the column
  }__attribute__((packed));
  
  
  struct ResultMetaDataPacket {
    PacketType type;
    ResultStatus status;
    uint32_t numColumns;
    uint32_t resultPacketSize; // This is the size of ResultPacket associated with this one
    ResultColumn columns[MAX_NUM_COLUMNS];  // Array containing name of each column and SQL type associated with it
    uint8_t terminator;
  }__attribute__((packed));
  
  /**
   * Used to return results in both queries and commands
   * for query see comments beside field
   * for command
   *  resultSize // number of rows affected
   *  status // status
   *  rowLen // errorCode
   */
  struct ResultPacket {
      PacketType type;
      ResultStatus status;  //16 bits
      uint16_t rowLen;       // Length of the row in columns, number of columns in the row
      uint32_t resultSize;  // Size in bytes that he data array will be
      uint64_t* data;
      uint8_t terminator;
  } __attribute__((packed));

  // TODO: Get rid of this when Neils data types are put in.
  enum SQL_TYPE {
    BOOLEAN
  };

  typedef std::vector<int64_t> ResultRow;

  typedef struct MetaDataColumn {
    std::string label;
    SQL_TYPE type; //TODO Make this a SQL_TYPE from Neils types
    uint16_t sqlType;
  } MetaDataColumn;

  
  /**
   * Class that represents the meta-data of a result.
   * Provides some basic information about the related result.
   */
  class ResultMetaData {
   public:

    /**
     * Gets the number of columns in the result set.
     */
    uint32_t getColumnCount();
    


    /**
     * Gets the label of the column at the index passed in
     * @param index The index of the column to get the label of
     * @return The label of the column at the index
     */
    std::string getColumnLabel(int index);



    /**
     * Gets the type of the data stored in the column at the index 
     * passed in
     * @param index The index of the column to get the label of
     * @return The type of the data stored at the index.
     */ 
    SQL_TYPE getColumnType(int index); //TODO: Replace with neils types

    /**
     * Builds a ResultMetaData object from passed in values
     * @data
     */
    static ResultMetaData buildResultMetaDataObject(std::vector<MetaDataColumn> data);

   private:
    /** Private constructor */
    ResultMetaData(std::vector<MetaDataColumn> m_data);

    /** The actual meta data */
    std::vector<MetaDataColumn> m_data;   
  };

  

  /**
   * Represents the result of a database command
   */
  class Result {
   public:
    
    /**
     * Gets the meta data object related to the result.
     * @return The meta data object that describes the result.
     */
    ResultMetaData getMetaData();


    /**
     * Gets the value of the column at the passed in index
     * @param index The index of the column to get the value of
     */
    int64_t getValue(int index);


    /**
     * Gets the next row in the result set
     * @returns true if the next row exists false otherwise.
     */
    bool next();            

    /**
     * Creates Result object from passe in values
     * @param rows The rows to include in the the Result object
     * @param metaData The metaData object for the Result
     */
    static Result buildResultObject(std::vector<ResultRow> rows,
                                    ResultMetaData metaData);

   private:
    Result(std::vector<ResultRow> rows, ResultMetaData resultMetaData);

    /** The rows returned from the database */
    std::vector<ResultRow> m_rows;
    
    /** The meta-data object that describes the results */
    ResultMetaData m_metaData;
  };

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
    CommandResult executeCommand(std::string command);


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

#endif //OPENMEMDB_LIBOMDB_H
