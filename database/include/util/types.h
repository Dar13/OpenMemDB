/**
 *
 */

#ifndef OMDB_UTIL_TYPES_H
#define OMDB_UTIL_TYPES_H

/**
 *  @brief Templated wrapper class that should handle Tervel's bit-fiddling
 *         graciously.
 *
 *  @detail This avoid both bitfields and bit-shifting and is both more
 *          generic and powerful, as template specializations are possible
 *          but probably not needed.
 */
template <class T>
struct TervelWrapper
{
    uint8_t reserved;
    T data;
};

enum DataType : uint8_t
{
    BOOLEAN = 0,
    SMALL_INT,
    INTEGER,
    BIG_INT,
    FLOAT,
    DATE,
    TIME
};

/**
 *  @brief A discriminated union of possible data types usable in a Tervel container
 *
 *  @detail This is necessary because of Tervel's containers only holding 64 bits.
 */
union Data
{
    // Any warnings thrown from this line need to carefully evaluated.
    // This currently holds values up to 7, and the max value defined in DataType
    // is 6.
    DataType type : 3;

    struct
    {
        float data;
        uint64_t pad : 26;
    } float_data;

    struct
    {
        uint16_t data;
        uint64_t pad : 42;
    } short_data;
    
    struct
    {
        uint32_t data;
        uint64_t pad : 26;
    } int_data;

    struct
    {
        uint64_t data : 58;
    } long_data;

    struct
    {
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint64_t pad : 26;
    } date_data;

    struct
    {
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        int8_t tz_hour;
        int8_t tz_minute;
        uint64_t pad : 18;
    } time_data;

// Apparently the attribute is unnecessary.
} /*__attribute__((PACKED))*/ ;

/**
 *  @brief A wrapper of the Data union to be used in Tervel. This data type must
 *  not be larger than 64 bits in size.
 */
union TervelData
{
    struct 
    {
        uint64_t tervel_status : 3;
        Data value;
    } data;

    uint64_t value;

// Apparently the attribute is unnecessary.
} /*__attribute__((PACKED))*/ ;

#endif
