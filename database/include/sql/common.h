/*
 *
 */

#ifndef OMDB_SQL_COMMON_H
#define OMDB_SQL_COMMON_H

#include <cstdint>

namespace omdb
{
}

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
