/*
 *
 */

#include <sql/types/boolean.h>

SQLBoolean::SQLBoolean()
    : SQLNullable(true, true), m_value(SQL_UNKNOWN)
{}

SQLBoolean::SQLBoolean(const SQLBoolean& other)
    : SQLNullable(other.IsNull(), other.IsNullable()), m_value(other.m_value)
{}

SQLBoolean::SQLBoolean(SQLBoolean& other)
    : SQLNullable(other.IsNull(), other.IsNullable()), m_value(other.m_value)
{}

SQLBoolean::SQLBoolean(bool value)
    : SQLNullable(false, true), m_value(value ? SQL_TRUE : SQL_FALSE)
{}

SQLBoolean::SQLBoolean(TervelData value)
    : SQLNullable(false, true)
{
    if(value.data.null == 1 || value.data.type == BOOLEAN)
    {
        m_is_null = true;
        return;
    }

    // TODO: Convert to BooleanData and then pull it out?
    m_value = (BooleanValue)(value.data.value);
}

SQLBoolean::SQLBoolean(BooleanValue value)
    : SQLNullable(false), m_value(value)
{}

SQLBoolean::SQLBoolean(BooleanValue value, bool nullable)
    : SQLNullable(false, nullable), m_value(value)
{}

bool SQLBoolean::IsUnknown() const
{
    return (IsNull() || m_value == SQL_UNKNOWN);
}

bool SQLBoolean::IsTrue() const
{
    return (!IsNull() && m_value == SQL_TRUE);
}

SQLBoolean& SQLBoolean::operator=(SQLBoolean other)
{
    this->m_value = other.m_value;
    return *this;
}

SQLBoolean operator!(SQLBoolean value)
{
    switch(value.m_value)
    {
        case SQL_FALSE:
            return SQL_TRUE;
        case SQL_TRUE:
            return SQL_FALSE;
        case SQL_UNKNOWN:
        default:
            return SQL_UNKNOWN;
    }
}

SQLBoolean operator||(const SQLBoolean& lhs, const SQLBoolean& rhs)
{
    if(lhs.IsUnknown() || rhs.IsUnknown())
    {
        return SQLBoolean(SQL_UNKNOWN);
    }
 
    return (lhs.m_value == SQL_TRUE) || (rhs.m_value == SQL_TRUE);
}

SQLBoolean operator&&(const SQLBoolean& lhs, const SQLBoolean& rhs)
{
    return !((!lhs) || (!rhs));
}


SQLBoolean operator==(const SQLBoolean& lhs, const SQLBoolean& rhs)
{
    if(lhs.IsUnknown() || rhs.IsUnknown())
    {
        return SQLBoolean(SQL_UNKNOWN);
    }
  
    if(lhs.m_value == rhs.m_value)
    {
        return SQLBoolean(SQL_TRUE);
    }
    else
    {
        return SQLBoolean(SQL_FALSE);
    }
}

SQLBoolean operator!=(const SQLBoolean& lhs, const SQLBoolean& rhs)
{
    return !(lhs == rhs);
}

SQLBoolean operator<(const SQLBoolean& lhs, const SQLBoolean& rhs)
{
    if(lhs.IsUnknown() || rhs.IsUnknown())
    {
        return SQLBoolean(SQL_UNKNOWN);
    }
  
    if(lhs.m_value < rhs.m_value)
    {
        return SQLBoolean(SQL_TRUE);
    }
    else
    {
        return SQLBoolean(SQL_FALSE);
    }
}

SQLBoolean operator<=(const SQLBoolean& lhs, const SQLBoolean& rhs)
{
    return !(lhs > rhs);
}

SQLBoolean operator>(const SQLBoolean& lhs, const SQLBoolean& rhs)
{
    return rhs < lhs;
}

SQLBoolean operator>=(const SQLBoolean& lhs, const SQLBoolean& rhs)
{
    return !(lhs < rhs);
}
