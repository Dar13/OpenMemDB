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

#ifndef OMDB_UTIL_TYPES_H
#define OMDB_UTIL_TYPES_H

/**
 *  \brief Enumeration of the data types supported by the database.
 */
enum DataType : int8_t
{
    NONE = -1,
    BOOLEAN = 0,
    SMALL_INT,
    INTEGER,
    BIG_INT,
    FLOAT,
    DATE,
    TIME
};

/* The following classes use a technique called "type punning" to efficiently
 * store data while still allowing for easy access to it via the struct
 * member interface. */

/**
 *  \brief Representation of the boolean data type in memory
 */
union BooleanData
{
    struct
    {
        uint8_t data;
        uint64_t pad : 56;
    };

    uint64_t value;
};

/**
 *  \brief Representation of the float data type in memory
 */
union FloatData
{
    struct
    {
        float data;
        uint32_t pad;
    };

    uint64_t value;
};

/**
 *  \brief Representation of the short integer (16-bit integer) data type in memory
 */
union ShortData
{
    struct
    {
        int16_t data;
        uint64_t pad : 48;
    };

    uint64_t value;
};

/**
 *  \brief Representation of the integer (32-bit integer) data type in memory
 */
union IntData
{
    struct
    {
        int32_t data;
        int32_t pad;
    };

    uint64_t value;
};

/**
 *  \brief Representation of the long integer (64-bit integer) data type in memory
 */
struct LongData
{
    int64_t value;
};

/**
 *  \brief Representation of an ISO 8601 date in memory
 */
union DateData
{
    struct
    {
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint32_t pad;
    };

    uint64_t value;
};

/**
 *  \brief Representation of an ISO 8601 time in memory
 */
union TimeData
{
    struct
    {
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        int8_t tz_hour;
        int8_t tz_minute;
        uint32_t pad : 24;
    };

    uint64_t value;
};

/**
 *  @brief A wrapper of the Data union to be used in Tervel. This data type must
 *  not be larger than 64 bits in size.
 */
union TervelData
{
    struct 
    {
        int64_t tervel_status : 3;  //!< Tervel requires this be given to them
        uint64_t type : 3;          //!< This is mapped to \refer DataType
        uint64_t null : 1;          //!< Set if the value is NULL

        uint64_t data_value : 57;        //!< 
    };

    int64_t value;
} __attribute__((packed));

// If this triggers, something has gone really wrong
static_assert(8 == sizeof(TervelData), "TervelData must be exactly 64 bits long");

/**
 *  \brief Defines a lower and upper bound for a range of data
 */
struct DataRange
{
    TervelData start;
    TervelData end;
};

#endif
