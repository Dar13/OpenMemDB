/*
 *
 */

#ifndef OMDB_RESULT_H
#define OMDB_RESULT_H

enum class ResultStatus : uint32_t
{
    SUCCESS = 0,
    ERROR_MEM_ALLOC,
    ERROR_INVALID_TABLE,
    ERROR_INVALID_DATA,
    ERROR_INVALID_RECORD,
    ERROR_SYNTAX,
    ERROR_INVALID_COLUMN
};

// TODO: Refactor so that this paradigm is used throughout
template <typename T>
struct Result
{
    Result(ResultStatus s, T res) : status(s), result(res) {}
    ResultStatus status;
    T result;
};

// Common result types
using UintResult = Result<uint32_t>;
using Uint64Result = Result<uint64_t>;

#endif
