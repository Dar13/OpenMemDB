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


/*
 * ResultSet.h
 *
 *  Created on: Oct 14, 2015
 *      Author: Mike
 */

#ifndef LIBMEM_RESULTSET_H_
#define LIBMEM_RESULTSET_H_

#include <cstdint>
#include <vector>
#include <string>

#include <boost/variant.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/variant.hpp>

namespace libmem {

  enum ResultCode {
	  RESULT_SUCCESS = uint16_t(0),
	  RESULT_EMPTY = uint16_t(1),
	  RESULT_ERROR = uint16_t(2)
	};

	enum ResultType {
	  QUERY,
	  INSERT,
	  UPDATE,
	  CREATE,
	  DELETE
	};


	/**
	 * Variant for SQLType
	 */
	typedef boost::variant<int, std::string> sql_type;

	/**
	 * Visitor function for accessing sql_type value
	 * @returns value of the variant.
	 */
	class get_value : public boost::static_visitor<> {
	 public:
	  template <typename T>
	  T operator() (T & operand) const {
	    return operand;
	  }
	};


	/**
	 * Key value pair representing a field in a database entry
	 * and it's corresponding value.
	 */
	class ResultPair {
	 public:

	  /**
	   * Constructor for a ResultPair.
	   * @param name
	   * @param value
	   */
	  ResultPair(std::string name, sql_type value)
	     :m_name(name), m_value(value) {}
//	    ResultPair(std::string name, SQLType<T> value)
//	     :m_name(name), m_value(value) {}

	  ResultPair()
	   : m_name(""), m_value((uint32_t)0) 
      {}

	   ~ResultPair(){}

	   /**
	    * Gets the name of the pair
	    * @return The name of the pair.
	    */
	   std::string get_name() {
	     return this->m_name;
	   }

	   /**
	    * Gets the value of the ReusltPair
	    * @return The value associated with the ResultPair
	    */
	   sql_type get_value() {
	     return this->m_value;
	   }

	 private:
	   friend class boost::serialization::access;
	       template <typename Archive>
	       void serialize(Archive & ar, const unsigned int version) {
	         ar & m_name;
	         ar & m_value;
	       }

	   std::string m_name;
	   sql_type m_value;
	 };

	/**
	 * Represents row of results.
	 */
//	typedef std::vector<ResultPair> ResultRow;

	/**
	 * Class to represent a row of results in a result set.
	 * Needs to be a class of it's own so that serialization can take place
	 */
	class ResultRow {
	 public:
	  friend class boost::serialization::access;
	    template <typename Archive>
      void serialize(Archive & ar, const unsigned int version) {
	      ar & m_result_row;
	    }


	  std::vector<ResultPair> m_result_row;

	 private:

	};



	
	/**
	 * Class that represents the results of a SQL command.
   *
   * @note If the command was something other than a query then the
   * m_result_set member variable will be null.
   *
	 */
	class ResultSet {

	 public:
	  /**
	   * Constructor for result set.
	   * @param result_code The connection code returned from the socket.
	   * @param size The number of rows in the result set.
	   * @param result_type The type of the result.
	   * @param result_set The actual set of results.
	   */
	  ResultSet(ResultCode result_code, uint32_t size, ResultType result_type,
	            std::vector<ResultRow> result_set)
	 : m_result_code(result_code), m_result_type(result_type),
	   m_result_set(result_set), m_size(size), m_current_row(0){}


	  ResultSet()
	  : m_result_code(RESULT_ERROR), m_current_row(0),
	    m_result_type(QUERY), m_size(0), m_result_set(0){}

	  ~ResultSet(){}

	  /**
	   * Gets the next row in the result set.
	   * @return The next row in the ResultSet
	   */
	  ResultRow get_next_row() {
		  ResultRow next_row = this->m_result_set.back();
		  this->m_result_set.pop_back();
		  return next_row;
		}


	  /**
	   * Gets the number of rows in the result set.
	   * @return
	   */
		uint32_t get_size() {
		  return this->m_result_set.size();
		}

		/**
		 * Returns the result type of ResultSet.
		 * @return
		 */
		ResultType get_result_type() {
		  return this->m_result_type;
		}


		/**
		 * Gets the result code of the result set.
		 * @return
		 */
		ResultCode get_result_code() {
		  return this->m_result_code;
		}

	 private:

		friend class boost::serialization::access;
		template <typename Archive>
		void serialize(Archive & ar, const unsigned int version) {
		  ar & m_result_code;
		  ar & m_size;
		  ar & m_result_type;
		  ar & m_current_row;
		  ar & m_result_set;
		}



		ResultCode m_result_code;
		uint32_t m_size;
		ResultType m_result_type;
		uint32_t m_current_row;
		std::vector<ResultRow> m_result_set;
	};


}

#endif /* LIBMEM_RESULTSET_H_ */
