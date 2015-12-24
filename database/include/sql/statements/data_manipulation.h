/* TODO: File header */

#ifndef DATA_MANIP_H
#define DATA_MANIP_H

#include <vector>
#include <string>

// Project includes
#include "data/data_store.h"

#include "util/types.h"

#include "sql/common.h"

// Forward declaration
struct Expression;

struct DataRange
{
    Data start;
    Data end;
};

class ExpressionValue
{
public:
    bool is_range;

    Data getValue() { return value; }
    DataRange getRange() { return (DataRange){range_start, range_end}; }

private:
    Data value;
    Data range_start;
    Data range_end;
};

struct Predicate
{
    // TODO: Do a profile run on this virtual, it may be too expensive
    virtual bool evaluate();
};

struct NestedPredicate : public Predicate
{
    ExpressionOperation op;
    Predicate* left_child;
    Predicate* right_child;

    bool evaluate();
};

struct ValuePredicate : public Predicate
{
    ExpressionOperation op;
    ColumnReference column;
    ExpressionValue expected_value;

    bool evaluate();
};

struct ColumnPredicate : public Predicate
{
    ExpressionOperation op;
    ColumnReference left_column;
    ColumnReference right_column;

    bool evaluate();
};

struct SelectQuery
{
    /* TODO (if needed): std::vector<Table*> source_tables;*/
    std::vector<ColumnReference> source_columns;
    std::vector<std::string> output_columns;
    std::vector<Predicate*> predicates;
};

// TODO: UPDATE, INSERT INTO & DELETE statements

#endif
