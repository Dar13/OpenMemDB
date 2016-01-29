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

#ifndef TOKEN_H
#define TOKEN_H

#include <string>

#include "util/types.h"

// TODO: Document this
struct TokenData
{
    TokenData()
        : text(nullptr), is_value(false), is_operation(false), is_column(false)
    {}

    TokenData(std::string* str)
        : text(str), is_value(false), is_operation(false), is_column(false)
    {}

    TokenData(std::string* str, bool val)
        : text(str), is_value(true), is_operation(false), is_column(false)
    {
        value.data.type = BOOLEAN;
        if(val)
        {
            value.data.value = 1;
        }
        else
        {
            value.data.value = 0;
        }
    }

    TokenData(std::string* str, int16_t val)
        : text(str), is_value(true), is_operation(false), is_column(false)
    {
        value.data.type = SMALL_INT;
        value.data.value = val;
    }
    
    TokenData(std::string* str, int32_t val)
        : text(str), is_value(true), is_operation(false), is_column(false)
    {
        value.data.type = INTEGER;
        value.data.value = val;
    }

    TokenData(std::string* str, int64_t val)
        : text(str), is_value(true), is_operation(false), is_column(false)
    {
        value.data.type = BIG_INT;
        value.data.value = val;
    }

    TokenData(std::string* str, float val)
        : text(str), is_value(true), is_operation(false), is_column(false)
    {
        value.data.type = FLOAT;
        value.data.value = static_cast<int64_t>(val);
    }

    TokenData(std::string* str, std::string* table, std::string* column)
        : text(str), is_value(false), is_operation(false), is_column(true),
        table_name(table), column_name(column)
    {}

    std::string* text;

    bool is_value;
    TervelData value;

    bool is_operation;
    uint16_t operation;

    bool is_column;
    std::string* table_name;
    std::string* column_name;
};

/**
 *  \brief The datatype passed into the parser from the tokenizer
 */
typedef TokenData* Token;

#endif
