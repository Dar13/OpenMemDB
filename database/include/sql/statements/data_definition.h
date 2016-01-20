/* TODO: File Header */

#ifndef DATA_DEF_H
#define DATA_DEF_H

// C++ standard library includes
#include <vector>
#include <string>

// Project includes
#include "sql/common.h"
#include "sql/statements/common.h"
#include "util/types.h"

struct CreateTableCommand : public ParsedStatement
{
    CreateTableCommand() : ParsedStatement(SQLStatement::CREATE_TABLE) {}
    std::string table_name;
    std::vector<SQLColumn> columns;
};

struct DropTableCommand : public ParsedStatement
{
    DropTableCommand() : ParsedStatement(SQLStatement::DROP_TABLE) {}
    std::string table_name;
};

#endif
