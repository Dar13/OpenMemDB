//
// Created by mike on 2/22/16.
//

#include "omdb_lib.h"

/**
 * This test assumes that there is a table calles users
 * This table "exists" in the temp_server
 * The temp_server must be running before this test can be run
 */
int main ()
{
  // Connect to server // need to write test server first
  auto connection = libomdb::Connection::connect("localhost", 3490, "fakeDB");
  // Send request to server
  auto queryResult = connection.executeQuery("SELECT name from users;");
  // Check that results are as expected

  // Send another request to the server
  std::string insertCommand = "INSERT INTO users VALUES (fake name, 100, 10);";
  auto commandResult = connection.executeCommand(insertCommand);
  // Give result to user

  // Close the connection to the db
  connection.disconnect();
  return 0;
}
