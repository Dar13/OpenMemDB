/*
 *
 */

#ifndef SQL_INTEGER_H
#define SQL_INTEGER_H

// Project-specific includes
#include <sql/common.h>
#include <sql/types/boolean.h>

template <typename T>
class SQLExactNumeric : public SQLNullable
{
public:
  SQLExactNumeric()
    : SQLNullable(true, true), m_data(0) {}

  SQLExactNumeric(const SQLExactNumeric<T>& other)
    : SQLNullable(false, other.IsNullable()), m_data(other.m_data)
  {}

  SQLExactNumeric(T value, bool nullable = true)
    : SQLNullable(false, nullable), m_data(value)
  {}

  T m_data;

  SQLExactNumeric<T>& operator=(const SQLExactNumeric<T>& other)
  {
    this->m_data = other.m_data;

    return *this;
  }

};

template <typename T, typename S>
SQLBoolean operator==(const SQLExactNumeric<T>& lhs, SQLExactNumeric<S>& rhs)
{
  if(lhs.IsNull() || rhs.IsNull())
  {
    return SQLBoolean(SQL_UNKNOWN);
  }

  if(lhs.m_data == rhs.m_data)
  {
    return SQLBoolean(SQL_TRUE);
  }
  else
  {
    return SQLBoolean(SQL_FALSE);
  }
}

template <typename T, typename S>
SQLBoolean operator!=(const SQLExactNumeric<T>& lhs, const SQLExactNumeric<S>& rhs)
{
  return !(lhs == rhs);
}

// Integer types
typedef SQLExactNumeric<int16_t> SQLSmallInt;
typedef SQLExactNumeric<int32_t> SQLInteger;
typedef SQLExactNumeric<int64_t> SQLBigInt;

#endif
