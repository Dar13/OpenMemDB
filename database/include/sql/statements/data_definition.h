/* TODO: File Header */

#ifndef DATA_DEF_H
#define DATA_DEF_H

// C++ standard library includes
#include <vector>
#include <string>

// Project includes
#include "sql/common.h"

struct CreateTableCommand
{
  std::string table_name;
  std::vector<std::string> column_names;
  std::vector<SQLType> column_types;
  std::vector<SQLConstraint> column_constraints;
};

#endif
