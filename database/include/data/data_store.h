/* TODO: File header */

#ifndef DATA_STORE_H
#define DATA_STORE_H

// Tervel includes
#include <tervel/containers/wf/vector/vector.hpp>
#include <tervel/containers/wf/hash-map/wf_hash_map.h>

// Boost includes
#include <boost/variant.hpp>

// Project includes
#include "sql/types/common.h"
#include "sql/statements/data_definition.h"
#include "util/types.h"
#include "hash_functor.h"

// Some typedefs(C++11-style) so that we don't have all that meaningless
// namespace and template junk pop up everywhere.
// Table data definitions

using Record = tervel::containers::wf::vector::Vector<Data>;

using DataTable = tervel::containers::wf::vector::Vector<Record*>;

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



// TODO: An interface into the above defined types, as well as the management
//       of that memory.

/**
 *  @brief The interface into the data that is shared between all worker threads.
 */
class DataStore
{
public:
    enum Error : uint32_t
    {
        SUCCESS = 0,
        MEM_ALLOC,
        INVALID_SCHEMA,
        INVALID_TABLE
    };

    // TODO: Fix this
    DataStore()
        : table_name_mapping(64)
    {}

    Error createTable(CreateTableCommand table_info);

    Error deleteTable(std::string table_name);

    DataTable* getTable(std::string table_name);

    TableSchema* getTableSchema(std::string table_name);

    Error getRow(std::string table_name, uint32_t row_id, Record** record);

private:
    TableMap table_name_mapping;
};


#endif
