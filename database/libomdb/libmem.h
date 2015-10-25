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
 * libmem.h
 *
 *  Created on: Oct 14, 2015
 *  @author Mike McGee
 */

#ifndef LIBMEM_LIBMEM_H_
#define LIBMEM_LIBMEM_H_

#include <cstdint>
#include <string>

#include "resultset.h"
#include <boost/variant.hpp>

namespace libmem {

	uint32_t Connect(uint16_t port_number, std::string host_name,
	                 std::string database);

	void Disconnect(uint32_t socket_fd);

	ResultSet ExecuteSQL(std::string command, uint32_t socket_fd);

}


#endif /* LIBMEM_LIBMEM_H_ */
