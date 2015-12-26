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

struct SelectQuery
{
    /* TODO (if needed): std::vector<Table*> source_tables;*/
    std::vector<ColumnReference> source_columns;
    std::vector<std::string> output_columns;
    std::vector<Predicate*> predicates;
};

// TODO: UPDATE, INSERT INTO & DELETE statements

#endif
