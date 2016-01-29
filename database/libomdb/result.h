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
#ifndef RESULT_H
#define RESULT_H

// Standard library includes
#include <cstdint>
#include <vector>
#include <string>

// Project includes

namespace libomdb {
  
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
  
}

#endif /*RESULT_H */
