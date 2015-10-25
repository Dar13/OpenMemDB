/*
 *
 */

#ifndef OMDB_BOOLEAN_H
#define OMDB_BOOLEAN_H

#include <sql/common.h>

enum BooleanValue
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
  SQLBoolean(BooleanValue value);
  SQLBoolean(BooleanValue value, bool nullable);

  //! Stores the tri-state boolean value.
  BooleanValue m_value;

  SQLBoolean& operator=(SQLBoolean other);

  bool IsUnknown() const;
private:
};

SQLBoolean operator!(SQLBoolean value);

SQLBoolean operator==(const SQLBoolean& lhs, const SQLBoolean& rhs);
SQLBoolean operator!=(const SQLBoolean& lhs, const SQLBoolean& rhs);

SQLBoolean operator>(const SQLBoolean& lhs, const SQLBoolean& rhs);
SQLBoolean operator>=(const SQLBoolean& lhs, const SQLBoolean& rhs);

SQLBoolean operator<(const SQLBoolean& lhs, const SQLBoolean& rhs);
SQLBoolean operator<=(const SQLBoolean& lhs, const SQLBoolean& rhs);


#endif
