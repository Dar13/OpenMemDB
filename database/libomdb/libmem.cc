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
 * libmem.cc
 *
 *  Created on: Oct 17, 2015
 *  @author Mike McGee
 */
#include "libmem.h"
#include "connector.h"

/**
 * Connects to specified port number and database.
 * @param port_number The port number to connect to.
 * @param database The database to connect to.
 * @return The socket number created for communication.
 */
uint32_t libmem::Connect(uint16_t port_number, std::string host_name,
                         std::string database) {
  // TODO: Need to incorporate connection to specific DB.
   return connector::Connect(port_number, host_name);
}

void libmem::Disconnect(uint32_t socket_fd) {
  connector::Disconnect(socket_fd);
}

libmem::ResultSet libmem::ExecuteSQL(std::string command,
                                        uint32_t socket_fd) {
  //TODO: Add connections to individual databases.
  return *connector::SendMessage(command, socket_fd);
}
