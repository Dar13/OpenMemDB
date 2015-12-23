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

enum class ExpressionFlags : uint32_t
{
    EMPTY       = 0x0,
    COLUMN      = 0x1,
    VALUE       = 0x2,
    OPERATION   = 0x4,
    NESTED      = 0x8
};

struct Expression
{
    Expression() 
        : left(nullptr), right(nullptr), 
          op(ExpressionOperation::NO_OP), flags(ExpressionFlags::EMPTY) 
    {}
    Expression* left;
    Expression* right;
    ExpressionOperation op;
  
    ExpressionFlags flags;
  
    DataType value_type;
    Data value;
};

struct StatementBuilder
{
    StatementBuilder()
        : type(SQLStatement::INVALID), statement(nullptr)
    {}
  
    SQLStatement type;
    void* statement;
    Expression* expr_tree;
};

// Table management functions
void builderStartCreateTable(StatementBuilder* builder, Token table_name);
void builderStartDropTable(StatementBuilder* builder, Token table_name);

// Query management function
void builderStartSelectQuery(StatementBuilder* builder);
void builderGeneratePredicates(StatementBuilder* builder);
void builderAddSelectAllColumns(StatementBuilder* builder, Token table);
void builderAddQualifiedSelectColumn(StatementBuilder* builder,
                                     Token table, Token source_column,
                                     Token output_column);
void builderFinishSelectQuery(StatementBuilder* builder);

// Somewhat generic helper functions
void builderAddColumnName(StatementBuilder* builder, Token column_name);
void builderAddColumnType(StatementBuilder* builder, Token column_type);
void builderAddTableName(StatementBuilder* builder, Token table_name);
void builderStartNestedExpr(StatementBuilder* builder, Token operation);
void builderAddValueExpr(StatementBuilder* builder,
        Token operation, Token left_term, Token right_term);

// Overall builder management
void builderClean(StatementBuilder* builder);

#endif
