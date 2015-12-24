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
public:
    ExpressionOperation op;
};

struct NestedPredicate : public Predicate
{
public:
    Predicate* left_child;
    Predicate* right_child;
};

struct ValuePredicate : public Predicate
{
public:
    ColumnReference column;
    ExpressionValue expected_value;
};

struct ColumnPredicate : public Predicate
{
public:
    ColumnReference left_column;
    ColumnReference right_column;
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
