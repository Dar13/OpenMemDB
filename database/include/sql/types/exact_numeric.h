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

#ifndef SQL_EXACT_NUMERIC_H
#define SQL_EXACT_NUMERIC_H

// Project-specific includes
#include <sql/common.h>
#include <sql/types/boolean.h>

#include <util/types.h>

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

    SQLExactNumeric(TervelData value)
        : SQLNullable(false, true)
    {
        if(value.data.null == 1)
        {
            m_is_null = true;
            return;
        }

        switch(value.data.type)
        {
            case SMALL_INT:
                if(sizeof(T) == sizeof(SQLSmallIntType))
                {
                    ShortData short_data = static_cast<ShortData>(value.data.value);
                    m_data = short_data.value;
                }
                else
                {
                    m_is_null = true;
                }
                break;
            case INTEGER:
                if(sizeof(T) == sizeof(SQLIntegerType))
                {
                    IntData int_data = static_cast<IntData>(value.data.value);
                    m_data = int_data.value;
                }
                else
                {
                    m_is_null = true;
                }
                break;
            case BIG_INT:
                if(sizeof(T) == sizeof(SQLBigIntType))
                {
                    m_data = value.data.value;
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
