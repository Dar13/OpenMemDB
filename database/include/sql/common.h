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

#ifndef OMDB_SQL_COMMON_H
#define OMDB_SQL_COMMON_H

// C standard library includes
#include <cstdint>

// C++ standard library includes
#include <string>
#include <vector>

// Project includes
#include <util/types.h>
#include "statements/expression.h"

typedef uint16_t SQLSmallIntType;
typedef uint32_t SQLIntegerType;
typedef uint64_t SQLBigIntType;

// TODO: Redo this enum's naming
enum class SQLConstraintType : uint32_t
{
  SQL_NO_CONSTRAINT = 0,
  SQL_NOT_NULL,
  SQL_DEFAULT,
  SQL_UNIQUE,
  SQL_PRIMARY_KEY,
  SQL_FOREIGN_KEY,
  SQL_AUTO_INCREMENT
};

/**
 *  \brief A representation of a SQL constraint.
 *
 *  \detail This covers the following constraints: NOT NULL, DEFAULT, UNIQUE,
 *          PRIMARY_KEY, FOREIGN_KEY, AUTO_INCREMENT
 */
struct SQLConstraint
{
  SQLConstraintType type;
  uint16_t column_idx;
  TervelData value;
  const char* ref_table;
  uint16_t ref_column_idx;
};

// TODO: Document this
struct SQLColumn
{
    std::string name;
    DataType type;
    std::vector<SQLConstraint> constraint;
};

enum class SQLStatement : uint16_t
{
  INVALID = 0,
  CREATE_TABLE,
  DROP_TABLE,
  SELECT,
  UPDATE,
  INSERT_INTO,
  DELETE
};

enum SQLAsciiUsage
{
  SQL_SPACE       = 0x1,
  SQL_ALPHA       = 0x2,
  SQL_DIGIT       = 0x4,
  SQL_ALNUM       = (SQL_DIGIT | SQL_ALPHA),
  SQL_XDIGIT      = 0x8,
  SQL_UPPER_ALPHA = 0x20,
  SQL_IDENT       = 0x40,
  SQL_SYMBOL      = 0x80,
  SQL_NONE        = 0x0
};

extern const unsigned char sql_ascii_map[256];

inline bool isSQLIdentifierChar(unsigned char ch)
{
  return (sql_ascii_map[ch] & (SQL_IDENT | SQL_ALNUM)) != 0;
}

inline bool isSQLAlphabeticChar(unsigned char ch)
{
    return (sql_ascii_map[ch] & (SQL_ALPHA)) != 0;
}

inline bool isSQLSymbolChar(unsigned char ch)
{
  return (sql_ascii_map[ch] & SQL_SYMBOL) != 0;
}

inline bool isSQLNumericChar(unsigned char ch)
{
    return (sql_ascii_map[ch] & SQL_DIGIT) != 0;
}

int strnicmp(const char* left, const char* right, int n);

void initializeOperationMap();

/**
 *  \brief A base class that implements the nullable property in SQL.
 *
 *  \detail This is core of all the nullable types in SQL and implements some
 *          helper functions.
 */
class SQLNullable
{
  public:
    SQLNullable(bool null, bool is_nullable = true)
      : m_is_null(null), m_is_nullable(is_nullable)
    {}

    bool IsNull() const { return m_is_null;}

    bool IsNullable() const { return m_is_nullable; }
  protected:
    bool m_is_null;
    bool m_is_nullable;
};

#endif
