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

// STL Includes
#include <vector>
#include <string>

// Project includes
#include "data/table.h"

#include "util/types.h"

#include "sql/common.h"
#include "sql/predicate.h"
#include "sql/statements/common.h"

/**
 *  \brief A struct that holds internal data comprising a SELECT query
 */
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

    //! Vector of columns in the source table that are targeted
    std::vector<ColumnReference> source_columns;

    //! Tables that are targeted in this query
    std::vector<std::string> tables;

    //! Names of output columns
    std::vector<std::string> output_columns;

    //! The binary expression tree that determines which rows are selected
    Predicate* predicate;
};

/**
 *  \brief A struct that holds internal data comprising a INSERT INTO statement
 */
struct InsertCommand : public ParsedStatement
{
    InsertCommand() : ParsedStatement(SQLStatement::INSERT_INTO) {}

    //! The vector of data that represents the row being inserted
    std::vector<TervelData> data;

    //! The table that's being inserted into
    std::string table;
};

/**
 *  \brief A struct that holds internal data comprising a UPDATE statement
 */
struct UpdateCommand : public ParsedStatement
{
    UpdateCommand() : ParsedStatement(SQLStatement::UPDATE) {}

    //! The name of the target table
    std::string table;

    //! The schema of the target table
    TableSchema table_schema;

    //! The columns to be updated and their data
    std::vector<ColumnUpdate> columns;

    //! A full record
    RecordData full_record;

    //! The binary expression that determines which records are operated on
    Predicate* predicate;

    //! Is actually executable by the SQL Engine
    bool runnable;
};

/**
 *  \brief A struct that holds internal data comprising a DELETE statement
 */
struct DeleteCommand : public ParsedStatement
{
    DeleteCommand() : ParsedStatement(SQLStatement::DELETE) {}

    //! Name of the table to be operated on
    std::string table;

    //! The binary expression tree that determines which records are deleted
    Predicate* predicate;
};

#endif
