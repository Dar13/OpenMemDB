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

#include <tervel/util/tervel.h>
#include <workmanager/work_manager.h>

/* TODO: REMOVE THIS, JUST FOR TESTING */
#include <sql/omdb_parser.h>
#include <data/data_store.h>

/**
 *  @brief The entry point of the application.
 */
int main(int argc, char** argv)
{
  // TODO: Handle passed in parameters
  printf("Arguments passed in:\n");
  for(int i = 0; i < argc; i++)
  {
    printf("\t- %s\n", argv[i]);
  }

  tervel::Tervel* tervel_main = new tervel::Tervel(9);
  WorkManager work_manager(8, tervel_main);

  int32_t status = 0;

  status = work_manager.Initialize();
  if(status != WorkManager::E_NONE)
  {
    printf("Work manager failed to initialize!\n");
    printf("Error code = %d\n", status);
    return 1;
  }

  status = work_manager.Run();
  if(status != WorkManager::E_NONE)
  {
    printf("Work manager failed to run!\n");
    printf("Error code = %d\n", status);
    return 1;
  }

  printf("Shutting down program...\n");

  return 0;
}
