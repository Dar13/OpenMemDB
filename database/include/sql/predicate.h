/*
 *
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

// TODO: Document this
struct ColumnReference
{
    std::string table;
    uint32_t column_idx;
};

// TODO: Document this
struct Predicate
{
public:
    PredicateType type;
    ExpressionOperation op;
};

// TODO: Document this
struct NestedPredicate : public Predicate
{
public:
    NestedPredicate() : left_child(nullptr), right_child(nullptr) {}
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
    ColumnReference column;
    ExpressionValue expected_value;
};

// TODO: Document this
struct ColumnPredicate : public Predicate
{
public:
    ColumnReference left_column;
    ColumnReference right_column;
};

Predicate* getPredicateFromExpression(Expression* expr, DataStore* data_store);

void setupStatement(ParsedStatement* statement, Expression* expr, DataStore* data_store);

#endif
