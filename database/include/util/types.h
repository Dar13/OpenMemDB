/**
 *
 */

#ifndef OMDB_UTILS_TYPES
#define OMDB_UTILS_TYPES

namespace omdb
{
    /**
     *  @brief Templated wrapper class that should handle Tervel's bit-fiddling
     *         graciously.
     *
     *  @detail This avoid both bitfields and bit-shifting and is both more
     *          generic and powerful, as template specializations are possible
     *          but probably not needed.
     */
    template <class T>
    class TervelWrapper
    {
        uint8_t reserved;
        T data;
    };
}

#endif
