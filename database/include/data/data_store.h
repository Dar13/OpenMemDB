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
#include <functional>
#include <atomic>

// Tervel includes
#include <tervel/containers/wf/vector/vector.hpp>
#include <tervel/containers/wf/hash-map/wf_hash_map.h>

// Boost includes
#include <boost/variant.hpp>

// Project includes
#include "data/types.h"
#include "data/table.h"
#include "accessor.h"
#include "util/types.h"
#include "util/result.h"
#include "sql/types/common.h"
#include "sql/predicate.h"
#include "sql/statements/data_definition.h"
#include "sql/statements/data_manipulation.h"
#include "sql/common.h"

// The mack-daddy, the mapping of table names to tables/schemas.
using TableMap = tervel::containers::wf::HashMap<size_t, 
                                                 SchemaTablePair*, 
                                                 TableHashFunctor<size_t, SchemaTablePair*>>;

// Some common Result types for this module
using DataResult = Result<TervelData>;
using RecordResult = Result<RecordData>;
using MultiRecordResult = Result<MultiRecordData>;
using ManipResult = Result<ManipStatus>;
using ConstraintResult = Result<ConstraintStatus>;

template<>
struct Result<ManipStatus> : public ResultBase
{
    Result(ResultStatus s, ManipStatus res) 
        : ResultBase(s, ResultType::COMMAND), result(res), rows_affected(0)
    {}

    ManipStatus result;
    uint32_t rows_affected;
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
        : table_name_mapping(63)
    {}

    TervelVector<ValuePointer<Record>*> test;

    ManipResult createTable(CreateTableCommand table_info);

    ManipResult deleteTable(const std::string& table_name);

    UintResult getColumnIndex(const std::string& table_name, const std::string& column_name);

    SchemaResult getTableSchema(const std::string& table_name);

    ManipResult insertRecord(const std::string& table_name,
                             RecordData record);

    ManipResult updateRecords(Predicate* predicates, 
                             const std::string& table_name,
                             RecordData record);

    ManipResult deleteRecords(Predicate* predicates,
                              const std::string& table_name);

    MultiRecordResult getRecords(Predicate* predicates,
                                 const std::string& table_name);

private:
    bool getTablePair(std::string table_name, SchemaTablePair& pair);

    RecordCopy copyRecord(RecordVector& table, int64_t row_idx);
    RecordCopy copyRecord(Record* record);

    Record* updateRecord(const RecordCopy& old_record, RecordData& new_record);

    MultiRecordCopies searchTable(std::shared_ptr<DataTable>& table, ValuePredicate* value_pred);
    /*
    MultiTableRecordData searchTable(std::string table_first, std::string table_second, 
                                ColumnPredicate* col_pred);
                                */

    MultiTableRecordCopies searchTables(NestedPredicate* pred);

    RecordReferences searchTableForRefs(std::shared_ptr<DataTable>& table, 
            ValuePredicate* value_pred);
    RecordReferences searchTablesForRefs(NestedPredicate* pred);

    ConstraintResult schemaChecker(SchemaTablePair& table_pair, RecordData *record);

    TableMap table_name_mapping;
};


#endif
