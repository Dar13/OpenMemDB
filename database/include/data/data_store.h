/* TODO: File header */

#ifndef DATA_STORE_H
#define DATA_STORE_H

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

using DataTable = tervel::containers::wf::vector::Vector<Record*>;

// This is just a copy of a record
using RecordData = std::vector<TervelData>;

// TODO: Is there a more efficient way?
using MultiRecordData = std::vector<RecordData>;

// Schema definition
struct TableSchema
{
    std::vector<SQLColumn> columns;
};

struct SchemaTablePair
{
    SchemaTablePair(DataTable* t, TableSchema* s)
        : table(t), schema(s)
    {}

    DataTable* table;
    TableSchema* schema;
};

// The mack-daddy, the mapping of table names to tables/schemas.
using TableMap = tervel::containers::wf::HashMap<std::string, 
                                                 SchemaTablePair*, 
                                                 TableHashFunctor<std::string, SchemaTablePair*>>;


// TODO: Does this need to exist? Does a secondary error code?
enum class ManipStatus : uint32_t
{
    SUCCESS = 0,
    FAILURE
};

// Some common Result types for this module
using DataResult = Result<TervelData>;
using RecordResult = Result<RecordData>;
using MultiRecordResult = Result<MultiRecordData>;
using ManipResult = Result<ManipStatus>;
using SchemaResult = Result<TableSchema>;

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
    RecordData copyRecord(Record* record);

    MultiRecordData searchTable(DataTable* table, ValuePredicate* value_pred);

    TableMap table_name_mapping;
};


#endif
