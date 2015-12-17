/*
 *
 */

#ifndef SQL_EXACT_NUMERIC_H
#define SQL_EXACT_NUMERIC_H

// Project-specific includes
#include <sql/common.h>
#include <sql/types/boolean.h>

#include <util/types.h>

typedef uint16_t SQLSmallIntType;
typedef uint32_t SQLIntegerType;
typedef uint64_t SQLBigIntType;

template <typename T>
class SQLExactNumeric : public SQLNullable
{
public:
    SQLExactNumeric()
        : SQLNullable(true, true), m_data(0) {}
    
    SQLExactNumeric(const SQLExactNumeric<T>& other)
        : SQLNullable(false, other.IsNullable()), m_data(other.m_data) {}
    
    SQLExactNumeric(T value, bool nullable = true)
        : SQLNullable(false, nullable), m_data(value) {}

    SQLExactNumeric(Data value)
        : SQLNullable(false, true)
    {
        if(value.null == 1)
        {
            m_is_null = true;
            return;
        }

        switch(value.type)
        {
            case SMALL_INT:
                if(sizeof(T) == sizeof(SQLSmallIntType))
                {
                    m_data = value.short_data.data;
                }
                else
                {
                    m_is_null = true;
                }
                break;
            case INTEGER:
                if(sizeof(T) == sizeof(SQLIntegerType))
                {
                    m_data = value.int_data.data;
                }
                else
                {
                    m_is_null = true;
                }
                break;
            case BIG_INT:
                if(sizeof(T) == sizeof(SQLBigIntType))
                {
                    m_data = value.long_data.data;
                }
                else
                {
                    m_is_null = true;
                }
                break;
            // Incompatible types, set to null
            case FLOAT:
            case DATE:
            case TIME:
            case BOOLEAN:
            default:
                m_is_null = true;
                break;
        }
    }
    
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
