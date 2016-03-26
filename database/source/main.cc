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
#include <util/omdb_stdlib.h>

/**
 *  \brief The entry point of the application.
 */
int main(int argc, char** argv)
{
  int num_threads = 8;

  for(int i = 1; i < argc; i++)
  {
      if(strcmp(argv[i], "help") == 0)
      {
          printf("Thanks for using OpenMemDB!\n");
          printf("Usage: openmemdb [help] [<num_threads>]\n");
          printf("Defaults to using 8 threads if no parameter is found.\n");
          return 0;
      }

      std::string arg(argv[i]);
      try
      {
          num_threads = std::stoi(arg);
      }
      catch(const std::invalid_argument& inv_arg)
      {
          std::cout << "Invalid commandline argument! './openmemdb help' for usage "
              "instructions" << std::endl;
          std::cout << "Defaulting to 8 threads..." << std::endl;
          num_threads = 8;
      }
  }

  // TODO: Is the plus 1 needed?
  tervel::Tervel* tervel_main = new tervel::Tervel(num_threads + 1);
  WorkManager work_manager(num_threads, tervel_main);

  // Setup some signal handlers
  setSignalHandler(SIGINT, [&work_manager] (int) { work_manager.Abort(); });
  setSignalHandler(SIGTERM, [&work_manager] (int) { work_manager.Abort(); });
  setSignalHandler(SIGSEGV, [&work_manager] (int) { work_manager.Abort(); });
  setSignalHandler(SIGABRT, [&work_manager] (int) { work_manager.Abort(); });

  int32_t status = 0;

  // Initialize the work manager
  status = work_manager.Initialize();
  if(status != WorkManager::E_NONE)
  {
    printf("Work manager failed to initialize!\n");
    printf("Error code = %d\n", status);
    return 1;
  }

  // Run the server!
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

