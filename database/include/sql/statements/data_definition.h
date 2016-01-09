/* TODO: File Header */

#ifndef DATA_DEF_H
#define DATA_DEF_H

// C++ standard library includes
#include <vector>
#include <string>

// Project includes
#include "sql/common.h"
#include "util/types.h"

struct CreateTableCommand
{
  std::string table_name;
  std::vector<SQLColumn> columns;
};

struct DropTableCommand
{
  std::string table_name;
};

#endif
