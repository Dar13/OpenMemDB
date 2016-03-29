/* Copyright (c) 2016 Neil Moore, Jason Stavrinaky, Micheal McGee, Robert Medina
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies 
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef SQL_PREDICATE_H
#define SQL_PREDICATE_H

#include "statements/common.h"
#include "statements/expression.h"

class DataStore;

/* TODO: Consider moving this to a static heap to prevent dealing with
 * dynamic memory allocation errors and their propagation.
 */

enum class PredicateType : uint8_t {
    NESTED = 0,
    VALUE,
    COLUMN
};

// TODO: Document this
class ExpressionValue
{
public:
    ExpressionValue() : is_range(false) {}
    ExpressionValue(TervelData value) : is_range(false), value(value) {}
    ExpressionValue(TervelData start, TervelData end)
        : is_range(true), range_start(start), range_end(end)
    {}

    bool is_range;

    TervelData getValue() { return value; }
    DataRange getRange() { return (DataRange){range_start, range_end}; }

private:
    TervelData value;
    TervelData range_start;
    TervelData range_end;
};

SQLBoolean operator==(ExpressionValue& lhs, ExpressionValue& rhs);
SQLBoolean operator!=(ExpressionValue& lhs, ExpressionValue& rhs);

// TODO: Document this
struct Predicate
{
public:
    Predicate(PredicateType type) 
        : type(type), op(ExpressionOperation::NO_OP) 
    {}

    PredicateType type;
    ExpressionOperation op;
};

// TODO: Document this
struct NestedPredicate : public Predicate
{
public:
    NestedPredicate() 
        : Predicate(PredicateType::NESTED), 
        left_child(nullptr), right_child(nullptr)
    {}

    ~NestedPredicate()
    {
        delete left_child;
        delete right_child;
    }

    Predicate* left_child;
    Predicate* right_child;
};

// TODO: Document this
struct ValuePredicate : public Predicate
{
public:
    ValuePredicate()
        : Predicate(PredicateType::VALUE)
    {}

    ColumnReference column;
    ExpressionValue expected_value;
};

// TODO: Document this
struct ColumnPredicate : public Predicate
{
public:
    ColumnPredicate()
        : Predicate(PredicateType::COLUMN)
    {}

    ColumnReference left_column;
    ColumnReference right_column;
};

Predicate* getPredicateFromExpression(Expression* expr, DataStore* data_store);

void setupStatement(ParsedStatement* statement, Expression* expr, DataStore* data_store);

SQLBoolean evaluateOperation(ExpressionOperation op, ExpressionValue lhs, ExpressionValue rhs);

#endif
