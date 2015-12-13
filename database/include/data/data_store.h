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
#include "util/types.h"

// Some typedefs(C++11-style) so that we don't have all that meaningless
// namespace and template junk pop up everywhere.
/*
// Table data definitions
using DataVariant = boost::variant<SQLBoolean, SQLDate, SQLTime, SQLTimestamp,
                                  SQLSmallInt, SQLInteger, SQLBigInt>;

using TervDataVariant = TervelWrapper<DataVariant>;

using DataTableRecord = TervelWrapper<tervel::containers::wf::vector<TervDataVariant>>;

using DataTable = tervel::containers::wf::vector<DataTableRecord>;

// Schema definition
using TableSchema = int;

using SchemaTablePair = std::pair<DataTable, TableSchema>;

// The mack-daddy, the mapping of table names to tables/schemas.
using TableMap = tervel::containers::wf::HashMap<std::string, 
                                                 SchemaTablePair, 
                                                 TableHashFunctor<std::string, SchemaTablePair>>;

// TODO: An interface into the above defined types, as well as the management
//       of that memory.
*/

using DataVariant = boost::variant<SQLBoolean, SQLDate, SQLTime, SQLTimestamp, SQLSmallInt, SQLInteger, SQLBigInt>;
using DataTableRecord = tervel::containers::wf::vector::Vector<DataVariant>;
using DataTable = tervel::containers::wf::vector::Vector<DataTableRecord>;
using TableSchema = DataTableRecord;
using SchemaTablePair = std::pair<DataTable,TableSchema>;
using TableMap = tervel::containers::wf::HashMap<std::string, SchemaTablePair>;  

#endif
