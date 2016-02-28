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

#ifndef DATA_STORE_H
#define DATA_STORE_H

// STL includes
#include <map>
#include <atomic>

// Tervel includes
#include <tervel/containers/wf/vector/vector.hpp>
#include <tervel/containers/wf/hash-map/wf_hash_map.h>

// Boost includes
#include <boost/variant.hpp>

// Project includes
#include "hash_functor.h"
#include "util/types.h"
#include "util/result.h"
#include "sql/types/common.h"
#include "sql/predicate.h"
#include "sql/statements/data_definition.h"
#include "sql/statements/data_manipulation.h"

// Some typedefs(C++11-style) so that we don't have all that meaningless
// namespace and template junk pop up everywhere.

// Table data definitions
using Record = tervel::containers::wf::vector::Vector<int64_t>;

using RecordVector = tervel::containers::wf::vector::Vector<Record*>;

// This is just a copy of a record
using RecordData = std::vector<TervelData>;

/**
 *  \brief A struct that is a processed copy of a \refer Record object
 *
 *  \detail The processing that's done compared to a raw \refer RecordData is
 *  that the record ID is popped off the back and stored explicitly.
 *
 *  \note THIS IS FOR INTERNAL USE ONLY!
 */
struct RecordCopy
{
    RecordCopy() : id(0) {}
    RecordCopy(RecordData&& data)
    {
        id = data.back().data.value;
        data.pop_back();
        this->data = std::move(data);
    }

    uint64_t id;
    RecordData data;
};

// TODO: Is there a more efficient way?
using MultiRecordData = std::vector<RecordData>;

/**
 *  \brief A vector that holds multiple \refer RecordCopy objects
 *
 *  \detail THIS IS FOR INTERNAL USE ONLY!
 */
using MultiRecordCopies = std::vector<RecordCopy>;

/**
 *  \brief A mapping from a table name to a set of \refer RecordCopy objects
 *
 *  \detail THIS IS FOR INTERNAL USE ONLY
 */
using MultiTableRecordCopies = std::map<std::string, MultiRecordCopies>;

/**
 * \brief The schema that a table must adhere to.
 *
 * TODO: Table constraints?
 */
struct TableSchema
{
    //! A vector of column definitions according to the SQL standard
    std::vector<SQLColumn> columns;
};

/**
 *  \brief A struct defining a table stored in the data store.
 *
 *  \detail Holds a wait-free vector of pointers to data records as well as a record counter
 *  to be used for row identification.
 */
struct DataTable
{
    //! The constructor that initializes the record counter to a sane value
    DataTable() : record_counter(0) {}
    ~DataTable()
    {
        // Remove table contents
        int64_t table_len = records.size(0);
        if(table_len != 0)
        {
	        for(int64_t i = 0; i < table_len; i++)
	        {
	            // Get the current row pointer
	            Record* row = nullptr;
	            while(!records.pop_back_w_ra(row)) {}
	            delete row;
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
    SchemaTablePair(DataTable* t, TableSchema* s)
        : table(t), schema(s)
    {}

    //! The data that adheres to the schema
    std::shared_ptr<DataTable> table;

    //! The table's schema
    TableSchema* schema;
};

//! A typedef that allows returning multiple sets of records from multiple tables
using MultiTableRecordData = std::map<std::string, MultiRecordData>;

// The mack-daddy, the mapping of table names to tables/schemas.
using TableMap = tervel::containers::wf::HashMap<std::string, 
                                                 SchemaTablePair*, 
                                                 TableHashFunctor<std::string, SchemaTablePair*>>;

// Some common Result types for this module
using DataResult = Result<TervelData>;
using RecordResult = Result<RecordData>;
using MultiRecordResult = Result<MultiRecordData>;
using ManipResult = Result<ManipStatus>;
using SchemaResult = Result<TableSchema>;

template<>
struct Result<ManipStatus> : public ResultBase
{
    Result(ResultStatus s, ManipStatus res) : ResultBase(s, ResultType::COMMAND), result(res) {}

    ManipStatus result;
};

template<>
struct Result<RecordData> : public ResultBase
{
    Result(ResultStatus s, const RecordData& res) :
        ResultBase(s, ResultType::QUERY), result(res)
    {}

    RecordData result;
};

template<>
struct Result<MultiRecordResult> : public ResultBase
{
    Result(ResultStatus s, MultiRecordData res) :
        ResultBase(s, ResultType::QUERY), result(res)
    {}

    MultiRecordData result;
};

/**
 *  @brief The interface into the data that is shared between all worker threads.
 */
class DataStore
{
public:
    DataStore()
        : table_name_mapping(64)
    {}

    ManipResult createTable(CreateTableCommand table_info);

    ManipResult deleteTable(std::string table_name);

    UintResult getColumnIndex(std::string table_name, std::string column_name);

    SchemaResult getTableSchema(std::string table_name);

    DataResult updateData(Predicate* predicates,
                          std::string table_name,
                          uint32_t column_idx,
                          TervelData value);

    DataResult getData(Predicate* predicates,
                       std::string table_name,
                       uint32_t column_idx);

    ManipResult insertRecord(std::string table_name,
                             RecordData record);

    ManipResult updateRecord(Predicate* predicates, 
                             std::string table_name,
                             RecordData record);

    MultiRecordResult getRecords(Predicate* predicates,
                                 std::string table_name);

private:
    SchemaTablePair* getTablePair(std::string table_name);

	RecordCopy copyRecord(RecordVector& table, int64_t row_idx);
    RecordCopy copyRecord(Record* record);

    MultiRecordCopies searchTable(std::shared_ptr<DataTable>& table, ValuePredicate* value_pred);
    /*
    MultiTableRecordData searchTable(std::string table_first, std::string table_second, 
                                ColumnPredicate* col_pred);
                                */

	MultiTableRecordCopies searchTables(NestedPredicate* pred);

    void schemaChecker(TableSchema schema, std::vector<SQLColumn> column);

    TableMap table_name_mapping;
};


#endif
