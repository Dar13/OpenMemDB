/* TODO: File header */

#ifndef SQL_BUILDER_H
#define SQL_BUILDER_H

// Boost includes
#include <boost/variant.hpp>

// C++ standard library includes
#include <string>

// C standard library includes
#include <cstdint>

// Project includes
#include "../common.h"
#include "data_definition.h"
#include "data_manipulation.h"

struct StatementBuilder
{
  StatementBuilder()
    : type(SQLStatement::INVALID), statement(nullptr)
  {}

  SQLStatement type;
  void* statement;
};

void builderStartCreateTable(StatementBuilder* builder, std::string* table_name);
void builderAddColumnName(StatementBuilder* builder, std::string* column_name);
void builderAddColumnType(StatementBuilder* builder, std::string* column_type);

#endif
