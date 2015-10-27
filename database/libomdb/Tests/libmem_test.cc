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

#include "../libmem.h"


/*
 * libmem_test.cc
 *
 *  Created on: Oct 19, 2015
 *  @author Mike McGee
 */


//Main method that will test the execution of the libmem api
int main (int argc, char**argv) {
  // Use libmem to send a message to the server.

  //1. Connect to server [Test server is port 3490]
  int sock_fd = libmem::Connect(3490, "localhost", "");
  std::cout << "Connected" << std::endl;
  //2. Execute a SQLCommand
  libmem::ResultSet result_set = libmem::ExecuteSQL("SELECT * FROM Users",
                                                     sock_fd);

  // Try to print some of the result.
/*for (auto&& res: result_set) {
    for (auto&& pair: res) {
      std::cout << pair.get_name() << ":" << pair.get_value();
    }
    std::cout << std::endl;
  }*/

  std::cout << "Executed command" << std::endl;
  //3.Disconnect from database.
  libmem::Disconnect(sock_fd);
  std::cout << "Disconnected" << std::endl;

  return 0;
}


