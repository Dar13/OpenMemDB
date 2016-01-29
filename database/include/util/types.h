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

// TODO: Document this
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

// TODO: Document the classes below
union BooleanData
{
    struct
    {
        uint8_t data;
        uint64_t pad : 56;
    };

    uint64_t value;
};

union FloatData
{
    struct
    {
        float data;
        uint32_t pad;
    };

    uint64_t value;
};

union ShortData
{
    struct
    {
        int16_t data;
        uint64_t pad : 48;
    };

    uint64_t value;
};

union IntData
{
    struct
    {
        int32_t data;
        int32_t pad;
    };

    uint64_t value;
};

struct LongData
{
    int64_t value;
};

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
        int64_t tervel_status : 3;
        uint64_t type : 3;
        uint64_t null : 1;

        uint64_t value : 57;
    } data;

    int64_t value;
};

// TODO: Document this
struct DataRange
{
    TervelData start;
    TervelData end;
};

#endif
