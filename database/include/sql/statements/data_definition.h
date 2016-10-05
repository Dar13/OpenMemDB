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

#ifndef DATA_DEF_H
#define DATA_DEF_H

// C++ standard library includes
#include <vector>
#include <string>

// Project includes
#include "sql/common.h"
#include "sql/statements/common.h"
#include "util/types.h"

/**
 *  \brief A struct that holds internal data comprising a CREATE TABLE statement
 */
struct CreateTableCommand : public ParsedStatement
{
    CreateTableCommand() : ParsedStatement(SQLStatement::CREATE_TABLE) {}
    std::string table_name;
    std::vector<SQLColumn> columns;
};

/**
 *  /brief A struct that holds internal data comprising a DROP TABLE statement
 */
struct DropTableCommand : public ParsedStatement
{
    DropTableCommand() : ParsedStatement(SQLStatement::DROP_TABLE) {}
    std::string table_name;
};

#endif
