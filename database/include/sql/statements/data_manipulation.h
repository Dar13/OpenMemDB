/* TODO: File header */

#ifndef DATA_MANIP_H
#define DATA_MANIP_H

#include <vector>
#include <string>

// Project includes
#include "data/data_store.h"

#include "util/types.h"

#include "sql/common.h"
#include "sql/statements/common.h"

struct SelectQuery : public ParsedStatement
{
    SelectQuery() : ParsedStatement(SQLStatement::SELECT), predicate(nullptr) {}

    std::vector<ColumnReference> source_columns;
    std::vector<std::string> tables;
    std::vector<std::string> output_columns;
    Predicate* predicate;
};

// TODO: UPDATE, INSERT INTO & DELETE statements

#endif
