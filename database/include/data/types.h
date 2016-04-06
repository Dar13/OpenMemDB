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

#ifndef DATA_TYPES_H
#define DATA_TYPES_H

// STL includes
#include <vector>
#include <map>

// Tervel includes
#include <tervel/containers/wf/vector/vector.hpp>
#include <tervel/containers/wf/hash-map/wf_hash_map.h>

// Project includes 
#include "accessor.h"
#include "hash_functor.h"
#include "util/types.h"
#include "util/result.h"

// Some typedefs(C++11-style) so that we don't have all that meaningless
// namespace and template junk pop up everywhere.

template<typename T>
using TervelVector = tervel::containers::wf::vector::Vector<T>;

// Table data definitions
using Record = TervelVector<int64_t>;

using RecordVector = TervelVector<ValuePointer<Record>*>;

using KeyHashFunctor = std::hash<std::string>;

// This is just a copy of a record
using RecordData = std::vector<TervelData>;

/**
 *  \brief A struct that is a processed copy of a \refer Record object
 *
 *  \detail The processing that's done compared to a raw \refer RecordData is
 *  that the record ID is popped off the back and stored explicitly.
 *
 *  \note THIS IS FOR INTERNAL USE ONLY!
 */
struct RecordCopy
{
    RecordCopy() : id(0) {}
    RecordCopy(RecordData&& data)
    {
        id = data.back().data.value;
        data.pop_back();
        this->data = std::move(data);
    }

    uint64_t id;
    RecordData data;
};

struct RecordReference
{
    RecordReference() : id(0), ptr(nullptr) {}
    RecordReference(uint64_t i, ValuePointer<Record>* p) : id(i), ptr(p) {}

    uint64_t id;
    ValuePointer<Record>* ptr;
};

// TODO: Is there a more efficient way?
using MultiRecordData = std::vector<RecordData>;

/**
 *  \brief A vector that holds multiple \refer RecordCopy objects
 *
 *  \detail THIS IS FOR INTERNAL USE ONLY!
 */
using MultiRecordCopies = std::vector<RecordCopy>;

/**
 *  \brief A mapping from a table name to a set of \refer RecordCopy objects
 *
 *  \detail THIS IS FOR INTERNAL USE ONLY
 */
using MultiTableRecordCopies = std::map<std::string, MultiRecordCopies>;

/**
 *  \brief A vector that holds record ID and references instead of full records
 *
 *  \detail THIS IS FOR INTERNAL USE ONLY
 */
using RecordReferences = std::vector<RecordReference>;

//! A typedef that allows returning multiple sets of records from multiple tables
using MultiTableRecordData = std::map<std::string, MultiRecordData>;

// Some helper typedefs
using ManipResult = Result<ManipStatus>;
using MultiRecordResult = Result<MultiRecordData>;
using ConstraintResult = Result<ConstraintStatus>;

template<>
struct Result<ManipStatus> : public ResultBase
{
    Result(ResultStatus s, ManipStatus res) 
        : ResultBase(s, ResultType::COMMAND), result(res), rows_affected(0)
    {}

    Result()
        : ResultBase(ResultStatus::SUCCESS, ResultType::COMMAND),
        result(ManipStatus::SUCCESS), rows_affected(0)
    {}

    ManipStatus result;
    uint32_t rows_affected;
};

template<>
struct Result<RecordData> : public ResultBase
{
    Result(ResultStatus s, const RecordData& res) :
        ResultBase(s, ResultType::QUERY), result(res)
    {}

    RecordData result;
};

template<>
struct Result<MultiRecordData> : public ResultBase
{
    Result(ResultStatus s, MultiRecordData res) :
        ResultBase(s, ResultType::QUERY), result(res)
    {}

    Result()
        : ResultBase(ResultStatus::SUCCESS, ResultType::QUERY)
    {}

    MultiRecordData result;
};

#endif
