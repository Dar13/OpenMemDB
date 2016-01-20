/*
 *
 */

#ifndef OMDB_DATETIME_H
#define OMDB_DATETIME_H

// Standard Library includes
#include <cstdint>
#include <cassert>

// Project-specific includes
#include <sql/common.h>
#include <sql/types/boolean.h>

#include <util/types.h>

class SQLTimestamp;

/**
*  \brief Representation of the SQL:2003 DATE data type
*/
class SQLDate : public SQLNullable
{
public:
    SQLDate();
    SQLDate(const SQLDate& other);
    SQLDate(uint16_t year, uint8_t month, uint8_t day, bool nullable = true);
    SQLDate(TervelData value);
  
    //! Year
    uint16_t m_year;
  
    //! Month in the year
    uint8_t m_month;
  
    //! Day in the month
    uint8_t m_day;
  
    bool IsValid();
  
    SQLDate& operator=(SQLDate& other);
};

SQLBoolean operator==(const SQLDate& lhs, const SQLDate& rhs);
SQLBoolean operator!=(const SQLDate& lhs, const SQLDate& rhs);

SQLBoolean operator< (const SQLDate& lhs, const SQLDate& rhs);
SQLBoolean operator<=(const SQLDate& lhs, const SQLDate& rhs);

SQLBoolean operator> (const SQLDate& lhs, const SQLDate& rhs);
SQLBoolean operator>=(const SQLDate& lhs, const SQLDate& rhs);

/**
 *  \brief Representation of the SQL:2003 TIME data type
 */
class SQLTime : public SQLNullable
{
public:
    SQLTime();
    SQLTime(const SQLTime& time);
    SQLTime(uint16_t hour, uint16_t minute, uint32_t second, bool nullable = true);
    SQLTime(TervelData value);
  
    //! Hour of the day
    uint16_t m_hour;
  
    //! Minute of the hour
    uint16_t m_minute;
  
    //! Second of the minute
    uint32_t m_second;
    // TODO: Fraction of second support
  
    //! Timezone displacement from UTC in hours
    int16_t m_tz_hour;
  
    //! Amount of timezone displacement from UTC within hour.
    int16_t m_tz_minute;
  
    //! Time zone displacement is set if true
    bool m_with_timezone;
  
    void SetTimeZoneDisplacement(uint16_t hours, uint16_t minutes);
  
    bool IsValid();
  
    SQLTime& operator=(SQLTime& other);
};

SQLBoolean operator==(const SQLTime& lhs, const SQLTime& rhs);
SQLBoolean operator!=(const SQLTime& lhs, const SQLTime& rhs);

SQLBoolean operator< (const SQLTime& lhs, const SQLTime& rhs);
SQLBoolean operator<=(const SQLTime& lhs, const SQLTime& rhs);

SQLBoolean operator> (const SQLTime& lhs, const SQLTime& rhs);
SQLBoolean operator>=(const SQLTime& lhs, const SQLTime& rhs);

#endif
