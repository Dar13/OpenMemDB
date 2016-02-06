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

    ~SelectQuery()
    {
        if(predicate)
        {
            switch (predicate->type)
            {
                case PredicateType::NESTED:
                    delete reinterpret_cast<NestedPredicate*>(predicate);
                    break;
                default:
                    delete predicate;
                    break;
            }
        }
    }

    std::vector<ColumnReference> source_columns;
    std::vector<std::string> tables;
    std::vector<std::string> output_columns;
    Predicate* predicate;
};

struct InsertCommand : public ParsedStatement
{
    InsertCommand() : ParsedStatement(SQLStatement::INSERT_INTO) {}

    std::vector<TervelData> data;
    std::string table;
};

// TODO: UPDATE & DELETE statements

#endif
