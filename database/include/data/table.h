/* Copyright (c) 2016 Neil Moore, Jason Stavrinaky, Micheal McGee, Robert Medina
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies 
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef DATA_TABLE_H
#define DATA_TABLE_H

#include <atomic>

#include "accessor.h"
#include "data/types.h"

#include "sql/common.h"
#include "sql/types/common.h"

#include "util/result.h"

/**
 * \brief The schema that a table must adhere to.
 *
 * \detail Table constraints are currently not supported by OpenMemDB
 */
struct TableSchema
{
    //! A vector of column definitions according to the SQL standard
    std::vector<SQLColumn> columns;
};

using SchemaResult = Result<TableSchema>;

/**
 *  \brief A struct defining a table stored in the data store.
 *
 *  \detail Holds a wait-free vector of pointers to data records as well as a record counter
 *  to be used for row identification.
 */
struct DataTable
{
    //! The constructor that initializes the record counter to a sane value
    DataTable() : records(10000), record_counter(0) {}
    ~DataTable()
    {
        // Remove table contents
        int64_t table_len = records.size(0);
        if(table_len != 0)
        {
	        for(int64_t i = 0; i < table_len; i++)
	        {
	            // Get the current row pointer
                ValuePointer<Record>* row_ptr;
	            if(!records.pop_back_w_ra(row_ptr))
                {
                    delete row_ptr->ptr;
                }
                else 
                {
                    printf("Unable to pop record off of table! Potential memory leak!\n");
                }
	        }
        }
    }

    //! The wait-free vector of pointers to the records stored in this table
    RecordVector records;

    //! An atomic counter that allows for the database to distinguish between 
    //! two duplicate records
    std::atomic<uint64_t> record_counter;
};

/**
 *  \brief A struct that binds a table's schema to its data.
 */
struct SchemaTablePair
{
    SchemaTablePair() : table(nullptr), schema(nullptr) {}
    SchemaTablePair(DataTable* t, TableSchema* s)
        : table(t), schema(s)
    {}

    SchemaTablePair(const SchemaTablePair& pair)
        : table(pair.table), schema(pair.schema)
    {}

    //! The data that adheres to the schema
    std::shared_ptr<DataTable> table;

    //! The table's schema
    TableSchema* schema;
};

#endif
