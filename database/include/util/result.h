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

#ifndef OMDB_RESULT_H
#define OMDB_RESULT_H

enum class ResultStatus : uint16_t
{
    SUCCESS = 0,
    FAILURE,
    FAILURE_OUT_MEMORY,
};

enum class ResultType : uint8_t
{
    QUERY = 0,
    COMMAND,
    OTHER,
};

struct ResultBase
{
    ResultBase(ResultStatus s, ResultType t = ResultType::OTHER) : type(t), status(s) {}
    virtual ~ResultBase() {}

    ResultType type;
    ResultStatus status;
};

template <typename T>
struct Result : public ResultBase
{
    Result(ResultStatus s, T res) : ResultBase(s), result(res) {}

    T result;
};

// Common result types
using UintResult = Result<uint32_t>;
using Uint64Result = Result<uint64_t>;
using IntResult = Result<int32_t>;

#endif
