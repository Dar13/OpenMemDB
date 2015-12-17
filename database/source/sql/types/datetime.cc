/*
 *
 */

#include <sql/types/datetime.h>

// ------------------- SQLDate Definition -------------------------------------

/**
 *  \brief Default constructor, creates null SQL DATE object
 */
SQLDate::SQLDate()
    : SQLNullable(true, true), m_year(0), m_month(0), m_day(0)
{}

/**
 *  \brief Copy constructor.
 */
SQLDate::SQLDate(const SQLDate& other)
    : SQLNullable(other.IsNull(), other.IsNullable()),
      m_year(other.m_year), m_month(other.m_month), m_day(other.m_day)
{}

/**
 *  \brief Constructor, creates non-null SQL DATE object.
 *
 *  \param[in] year     Gregorian year
 *  \param[in] month    Month in the year
 *  \param[in] day      Day in the month
 *  \param[in] nullable Determines whether the object is nullable.
 */
SQLDate::SQLDate(uint16_t year, uint8_t month, uint8_t day, bool nullable)
    : SQLNullable(false, nullable),
      m_year(year), m_month(month), m_day(day)
{}

/**
 *  \brief Constructor, creates a nullable SQL DATE object.
 *
 *  \param[in] value    A Data variant to use when constructing this object
 */
SQLDate::SQLDate(Data value)
    : SQLNullable(false, true)
{
    if(value.null == 1 || value.type != DATE)
    {
        m_is_null = true;
        return;
    }

    m_year = value.date_data.year;
    m_month = value.date_data.month;
    m_day = value.date_data.day;
}

/**
 *  \brief Determines whether the calendar date represented by this object is
 *         valid in the Gregorian calendar.
 */
bool SQLDate::IsValid()
{
    if(IsNull())
    {
        return false;
    }
    
    // TODO: Verify the members are a legitimate Gregorian calendar date
    
    return true;
}

/**
 *  \brief Extracts a SQL DATE object from a SQL TIMESTAMP.
 *
 *  \param[in] timestamp  The SQL TIMESTAMP object
 */
SQLDate SQLDate::fromTimestamp(SQLTimestamp& timestamp)
{
    return SQLDate(timestamp.m_date);
}

/**
 *  \brief Assigns the given SQL DATE object value to this object
 */
SQLDate& SQLDate::operator=(SQLDate& other)
{
    if(other.IsNull())
    {
        this->m_is_nullable = true;
        this->m_is_null = true;
    }
    else
    {
        this->m_year = other.m_year;
        this->m_month = other.m_month;
        this->m_day = other.m_day;
    }
    
    return *this;
}

SQLBoolean operator==(const SQLDate& lhs, const SQLDate& rhs)
{
    if(lhs.IsNull() || rhs.IsNull())
    {
        return SQLBoolean(SQL_UNKNOWN);
    }
    
    if(lhs.m_year == rhs.m_year && lhs.m_month == rhs.m_month && lhs.m_day == rhs.m_day)
    {
        return SQLBoolean(SQL_TRUE);
    }
    else
    {
        return SQLBoolean(SQL_FALSE);
    }
}

SQLBoolean operator!=(const SQLDate& lhs, const SQLDate& rhs)
{
    return !(lhs == rhs);
}

SQLBoolean operator< (const SQLDate& lhs, const SQLDate& rhs)
{
    if(lhs.IsNull() || rhs.IsNull())
    {
        return SQLBoolean(SQL_UNKNOWN);
    }
    
    if(lhs.m_year < rhs.m_year || 
       (lhs.m_year == rhs.m_year && lhs.m_month < rhs.m_month) ||
       (lhs.m_year == rhs.m_year && lhs.m_month == rhs.m_month && lhs.m_day < rhs.m_day))
    {
        return SQLBoolean(SQL_TRUE);
    }
    else
    {
        return SQLBoolean(SQL_FALSE);
    }
}

SQLBoolean operator<=(const SQLDate& lhs, const SQLDate& rhs)
{
    return (lhs < rhs) || (lhs == rhs);
}

SQLBoolean operator> (const SQLDate& lhs, const SQLDate& rhs)
{
    return !(lhs <= rhs);
}

SQLBoolean operator>=(const SQLDate& lhs, const SQLDate& rhs)
{
    return !(lhs < rhs);
}

// ------------------- SQLTime Definition -------------------------------------

/**
 *  \brief Default constructor, creates null TIME object without a time zone.
 */
SQLTime::SQLTime()
    : SQLNullable(true, true), m_hour(0), m_minute(0), m_second(0), 
      m_with_timezone(false)
{}

/**
 *  \brief Copy constructor, handles copying from both SQL TIME with or without a
 *         timezone.
 *
 *  \param[in] time The SQL TIME object to copy.
 */
SQLTime::SQLTime(const SQLTime& time)
    : SQLNullable(time.IsNull(), time.IsNullable()),
      m_hour(time.m_hour), m_minute(time.m_minute), m_second(time.m_second),
      m_tz_hour(time.m_tz_hour), m_tz_minute(time.m_tz_minute),
      m_with_timezone(time.m_with_timezone)
{}

/**
 *  \brief Constructor, creates non-null SQL TIME object without time zone
 *         displacement.
 *
 *  \param[in] hour       The hour in the day.
 *  \param[in] minute     The minute in the hour.
 *  \param[in] second     The second in the minute.
 *  \param[in] nullable   Tells the data to be nullable or not.
 */
SQLTime::SQLTime(uint16_t hour, uint16_t minute, uint32_t second, bool nullable)
    : SQLNullable(false, nullable), 
      m_hour(hour), m_minute(minute), m_second(second),
      m_with_timezone(false)
{}

/**
 *  \brief Constructor, creates nullable SQL TIME object.
 *
 *  \param[in] value      The Data variant to use when constructing the object
 */
SQLTime::SQLTime(Data value)
    : SQLNullable(false, true)
{
    if(value.null == 1 || value.type != TIME)
    {
        m_is_null = true;
        return;
    }

    m_hour = value.time_data.hour;
    m_minute = value.time_data.minute;
    m_second = value.time_data.second;

    if(value.time_data.tz_hour != 0 || value.time_data.tz_minute != 0)
    {
        m_with_timezone = true;
        m_tz_hour = value.time_data.tz_hour;
        m_tz_minute = value.time_data.tz_minute;
    }
}

/**
 *  \brief Sets the timezone displacement for the time, making it a SQL TIME 
 *         object with a time zone.
 *
 *  \param[in] hours    The displacment in hours.
 *  \param[in] minutes  The displacment in minutes within the hour.
 */
void SQLTime::SetTimeZoneDisplacement(uint16_t hours, uint16_t minutes)
{
    m_with_timezone = true;
    m_tz_hour = hours;
    m_tz_minute = minutes;
}

/**
 *  \brief Determines if the time of day represented by this object is valid.
 */
bool SQLTime::IsValid()
{
    if(IsNull())
    {
        return false;
    }
  
    // TODO: Verify the members comprise a legitimate 24-hour time
    
    return true;
}

/**
 *  \brief Extract a SQL TIME from a timestamp without a time zone displacement.
 *
 *  \param[in] timestamp      The SQL TIMESTAMP object
 *  \param[in] with_timezone  Determines whether to return a SQL TIME with or
 *                            without a time zone displacement.
 */
SQLTime SQLTime::fromTimestamp(SQLTimestamp& timestamp, bool with_timezone)
{
    if(with_timezone)
    {
        // TODO: Requires conversion from timestamp no timezone to timestamp with timezone
        //
        // Formula goes:
        //  SourceTimestamp -> Timestamp with time zone -> Time with time zone
    }
    else
    {
        SQLTime ret_val(timestamp.m_time);
        ret_val.m_with_timezone = false;
  
        return ret_val;
    }
}

/**
 *  \brief Extract a SQL TIME from a timestamp with a time zone displacement.
 *
 *  \param[in] timestamp      The SQL TIMESTAMP object
 *  \param[in] with_timezone  Determines whether to return a SQL TIME with or
 *                            without a time zone displacment.
 */
SQLTime SQLTime::fromTimestampTZ(SQLTimestamp& timestamp, bool with_timezone)
{
    if(with_timezone)
    {
        SQLTime ret(timestamp.m_time);
        return ret;
    }
    else
    {
        // TODO: Requires conversion from timestamp with timezone to timestamp no timezone
        //
        // Formula goes:
        //  SourceTimestamp -> Timestamp w/o time zone -> Time w/o time zone
  
        return SQLTime();
    }
}

bool operator==(const SQLTime& lhs, const SQLTime& rhs)
{
    if(lhs.m_hour == rhs.m_hour && lhs.m_minute == rhs.m_minute &&
       lhs.m_second == rhs.m_second)
    {
        if(lhs.m_with_timezone && rhs.m_with_timezone)
        {
            if(lhs.m_tz_hour == rhs.m_tz_hour && lhs.m_tz_minute && rhs.m_tz_minute)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
  
        if(!lhs.m_with_timezone && !rhs.m_with_timezone)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool operator!=(const SQLTime& lhs, const SQLTime& rhs)
{
    return !(lhs == rhs);
}

bool operator< (const SQLTime& lhs, const SQLTime& rhs)
{
    assert(false);
}

bool operator<=(const SQLTime& lhs, const SQLTime& rhs)
{
    assert(false);
}

bool operator> (const SQLTime& lhs, const SQLTime& rhs)
{
    assert(false);
}

bool operator>=(const SQLTime& lhs, const SQLTime& rhs)
{
    assert(false);
}
