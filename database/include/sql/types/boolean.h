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

#ifndef OMDB_BOOLEAN_H
#define OMDB_BOOLEAN_H

#include <sql/common.h>

#include <util/types.h>

enum BooleanValue : uint8_t
{
    SQL_FALSE = 0,
    SQL_TRUE = 1,
    SQL_UNKNOWN = 2
};

class SQLBoolean : public SQLNullable
{
public:
    SQLBoolean();
    SQLBoolean(const SQLBoolean& other);
    SQLBoolean(SQLBoolean& other);
    SQLBoolean(bool value);
    SQLBoolean(TervelData value);
    SQLBoolean(BooleanValue value);
    SQLBoolean(BooleanValue value, bool nullable);
  
    //! Stores the tri-state boolean value.
    BooleanValue m_value;
  
    SQLBoolean& operator=(SQLBoolean other);
  
    bool IsUnknown() const;
    bool IsTrue() const;
private:
};

SQLBoolean operator!(SQLBoolean value);
SQLBoolean operator||(const SQLBoolean& lhs, const SQLBoolean& rhs);
SQLBoolean operator&&(const SQLBoolean& lhs, const SQLBoolean& rhs);

SQLBoolean operator==(const SQLBoolean& lhs, const SQLBoolean& rhs);
SQLBoolean operator!=(const SQLBoolean& lhs, const SQLBoolean& rhs);

SQLBoolean operator>(const SQLBoolean& lhs, const SQLBoolean& rhs);
SQLBoolean operator>=(const SQLBoolean& lhs, const SQLBoolean& rhs);

SQLBoolean operator<(const SQLBoolean& lhs, const SQLBoolean& rhs);
SQLBoolean operator<=(const SQLBoolean& lhs, const SQLBoolean& rhs);


#endif
