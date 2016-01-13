/*
 *
 */

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>

#include "sql/parser/token.h"
#include "util/types.h"

enum class ExpressionFlags : uint32_t
{
    EMPTY       = 0x0,
    COLUMN      = 0x1,
    VALUE       = 0x2,
    OPERATION   = 0x4,
    NESTED      = 0x8
};

ExpressionFlags operator|(ExpressionFlags a, ExpressionFlags b);
ExpressionFlags operator&(ExpressionFlags a, ExpressionFlags b);

bool isExprFlagContained(ExpressionFlags a, ExpressionFlags b);

enum class ExpressionOperation : uint16_t
{
    NO_OP = 0,
    EQUALS,
    NOT_EQUALS,
    LESSER,
    LESSER_EQUALS,
    GREATER,
    GREATER_EQUALS,
    IN,
    BETWEEN,
    AND,
    OR
};

struct Expression
{
    Expression();
    Expression(Token tok);

    ~Expression();

    Expression* left;
    Expression* right;
    ExpressionOperation op;
  
    ExpressionFlags flags;

    std::string* table_name;
    std::string* table_column;
  
    DataType value_type;
    TervelData value;
};

ExpressionOperation getOperation(std::string op);

std::string getOperationString(ExpressionOperation op);

void printExpressionTree(Expression* expr, int indent = 0);

#endif
