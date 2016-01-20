/*
 *
 */

#ifndef SQL_STMT_COMMON_H
#define SQL_STMT_COMMON_H

#include "sql/types/common.h"

class ParsedStatement
{
public:
    ParsedStatement(SQLStatement type) : type(type) {}
    SQLStatement type;
    uint32_t time_elapsed;
};

#endif
