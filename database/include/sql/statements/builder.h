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

#include "util/types.h"

struct StatementBuilder
{
    StatementBuilder()
        : type(SQLStatement::INVALID), statement(nullptr)
    {}
  
    SQLStatement type;
    void* statement;
};

enum class ExprFlags : uint32_t
{
    EMPTY       = 0x0,
    COLUMN_REF  = 0x1,
    LITERAL     = 0x2,
    COMPOUND    = 0x4
};

struct Expression
{
    Expression* left;
    Expression* right;
    ExpressionOperation op;
  
    ExprFlags flags;
  
    DataType value_type;
    void* value;
};

// Table management functions
void builderStartCreateTable(StatementBuilder* builder, SQLToken table_name);
void builderStartDropTable(StatementBuilder* builder, SQLToken table_name);

// Query management function
void builderStartSelectQuery(StatementBuilder* builder);
void builderAddSelectPredicate(StatementBuilder* builder, SQLToken expr);
void builderAddSelectAllColumns(StatementBuilder* builder, SQLToken table);
void builderAddQualifiedSelectColumn(StatementBuilder* builder,
                                     SQLToken table, SQLToken source_column,
                                     SQLToken output_column);
void builderFinishSelectQuery(StatementBuilder* builder);

// Somewhat generic helper functions
void builderAddColumnName(StatementBuilder* builder, SQLToken column_name);
void builderAddColumnType(StatementBuilder* builder, SQLToken column_type);
void builderAddTableName(StatementBuilder* builder, SQLToken table_name);
void builderAddExpressions(StatementBuilder* builder,
                           SQLToken left, SQLToken right, SQLToken op);


// Overall builder management
void builderClean(StatementBuilder* builder);

#endif
