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
#include "sql/types/common.h"
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

enum class ResultStatus : uint32_t
{
    SUCCESS = 0,
    ERR_MEM_ALLOC,
    INVALID_TABLE,
    INVALID_DATA,
    INVALID_RECORD
};

// TODO: Does this need to exist? Does a secondary error code?
enum class ManipStatus : uint32_t
{
    SUCCESS = 0,
    FAILURE
};

// TODO: Refactor so that this paradigm is used throughout
template <typename T>
struct Result
{
    Result(ResultStatus s, T res) : status(s), result(res) {}
    ResultStatus status;
    T result;
};

// Some common Result types

using DataResult = Result<TervelData>;
using RecordResult = Result<RecordData>;
using MultiRecordResult = Result<MultiRecordData>;
using ManipResult = Result<ManipStatus>;
using UintResult = Result<uint32_t>;

/**
 *  @brief The interface into the data that is shared between all worker threads.
 */
class DataStore
{
public:
    // TODO: Rename this
    enum class Error : uint32_t
    {
        SUCCESS = 0,
        MEM_ALLOC,
        INVALID_SCHEMA,
        INVALID_TABLE
    };

    DataStore()
        : table_name_mapping(64)
    {}

    ManipResult createTable(CreateTableCommand table_info);

    ManipResult deleteTable(std::string table_name);

    UintResult getColumnIndex(std::string table_name, std::string column_name);

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

    TableMap table_name_mapping;
};


#endif
