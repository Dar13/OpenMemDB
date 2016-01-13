/*
 *
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
