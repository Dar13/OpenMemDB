/*
 *
 */

#ifndef OMDB_SQL_COMMON_H
#define OMDB_SQL_COMMON_H

#include <cstdint>

enum SQLAsciiUsage
{
  SQL_SPACE       = 0x1,
  SQL_ALPHA       = 0x2,
  SQL_DIGIT       = 0x4,
  SQL_ALNUM       = (SQL_DIGIT | SQL_ALPHA),
  SQL_XDIGIT      = 0x8,
  SQL_UPPER_ALPHA = 0x20,
  SQL_IDENT       = 0x40,
  SQL_NONE        = 0x0
};

extern const unsigned char sql_ascii_map[256];

inline bool isSQLIdentifierChar(unsigned char ch)
{
  return (sql_ascii_map[ch] & SQL_IDENT) != 0;
}

int strnicmp(const char* left, const char* right, int n);

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
