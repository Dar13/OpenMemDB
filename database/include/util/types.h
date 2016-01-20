/**
 *
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
