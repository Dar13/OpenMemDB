/*
The MIT License (MIT)
Copyright (c) 2015 University of Central Florida's Computer Software Engineering
Scalable & Secure Systems (CSE - S3) Lab
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "omdb_lib.h"


/************************************************************************
 * ResultMetaData Implementations                                       *
 ************************************************************************/
libomdb::ResultMetaData::ResultMetaData(std::vector<MetaDataColumn> data)
  :m_data(data){}

uint32_t libomdb::ResultMetaData::getColumnCount() {
  return this->m_data.size();
}


std::string libomdb::ResultMetaData::getColumnLabel(int index) {
  return this->m_data.at(index).label;
}

// TODO: Replace return type with Neil's SQL_TYPE
libomdb::SQL_TYPE libomdb::ResultMetaData::getColumnType(int index) {
  return this->m_data.at(index).type;
}

libomdb::ResultMetaData libomdb::ResultMetaData::buildResultMetaDataObject(
    std::vector<MetaDataColumn> data) {
  libomdb::ResultMetaData* resultMetaData = new libomdb::ResultMetaData(data);
  return *resultMetaData;
}
/*************************************************************************
 * Result Implementations                                                *
 *************************************************************************/
libomdb::Result::Result(std::vector<ResultRow> rows,
                        ResultMetaData resultMetaData)
    :m_rows(rows), m_metaData(resultMetaData) {}


libomdb::Result libomdb::Result::buildResultObject(std::vector<ResultRow> rows,
                                                   ResultMetaData metaData) {
  libomdb::Result* result = new libomdb::Result(rows, metaData);
  return *result;
}


libomdb::ResultMetaData libomdb::Result::getMetaData() {
  return this->m_metaData;
}


int64_t libomdb::Result::getValue(int index) {
  // Accesses the back element because pop_back() is provided
  // by std. This will affect the ordering. 
  return this->m_rows.back().at(index);
}


bool libomdb::Result::next() {
  if (this->m_rows.empty()) {
    return false;  
  } else {
    this->m_rows.pop_back();
    return true;
  }
}
