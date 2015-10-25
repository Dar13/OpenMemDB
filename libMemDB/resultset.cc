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
#include <cstdlib>
#include <iostream>


#include "libmem.h"
#include "resultset.h"

/*
 * resultset.cc
 *
 *  Created on: Oct 17, 2015
 *  @author Mike McGee
 */



/**
 * @brief Constructor for ResultSet.
 *
 * @param result_code The result code from the SQL command.
 * @param size The number of rows in the result set.
 * @param result_type The type of command that produced this result
 * @param result_set The vector of ResultRows representing the data.
 */
libmem::ResultSet::ResultSet(ResultCode result_code, uint32_t size,
                             ResultType result_type,
                             std::vector<ResultRow> result_set)
  : m_result_code(result_code), m_size(size), m_result_type(result_type),
    m_result_set(result_set), m_current_row(0) {}



/**
 * @brief Destructor for ResulSet
 */
libmem::ResultSet::~ResultSet(){}



/**
 * @brief Gets the next row in the result set.
 *
 * @note Pops the result row off of the result set vector so it can not be accessed again
 * 		 after being retrieved.
 *
 * @return The next ResultRow in the result set.
 */
libmem::ResultRow libmem::ResultSet::get_next_row() {
//	if (this->m_current_row >= this->m_size) {
//		return NULL;
//	}

	this->m_current_row++;
	libmem::ResultRow next_row = this->m_result_set.back();
	this->m_result_set.pop_back();
	return next_row;
}



/**
 * @brief Gets the number of rows in the result set.
 * @return The number of rows in the result set.
 */
uint32_t libmem::ResultSet::get_size() {
  return this->m_result_set.size();
}



/**
 * @brief Gets the result type of the result set.
 * @return The result type of the result set.
 */
libmem::ResultType libmem::ResultSet::get_result_type() {
  return this->m_result_type;
}


/**
 * @brief Gets the result code for the result set.
 * @return The result code for the result set.
 */
libmem::ResultCode libmem::ResultSet::get_result_code() {
  return this->m_result_code;
}



/******************************************************************************
 * ResultPair implementations
 ******************************************************************************/

/**
 * @brief Constructor for result pairs.
 * @param name The name of the column
 * @param value The value associated with that column.
 */
libmem::ResultPair::ResultPair(std::string name, void* value)
  :m_name(name), m_value(value) {}


/**
 * @brief Destructor for ResultPair
 */
libmem::ResultPair::~ResultPair(){}


/**
 * @brief Gets the name of the column for the pair
 * @return The name of the pair
 */
std::string libmem::ResultPair::get_name() {
  return this->m_name;
}


/**
 * @brief Returns a pointer to the value of the column
 * @note Needs to be cast to the desired value type.
 *
 * @return Pointer to the value of the column.
 */
void* libmem::ResultPair::get_value() {
  return this->m_value;
}
