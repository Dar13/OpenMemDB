CC=g++

INC_DIR := -I../../

CFLAGS := -std=c++11 $(INC_DIR) -g -Wall -Wextra
LDFLAGS := -pthread -L ../../ -lomdb

SERVER_INC_DIR = -I../../ \
		 -I../../../include/

SERVER_CFLAGS := -std=c++11 $(SERVER_INC_DIR) -g -Wall -Wextra
SERVER_LDFLAGS := -pthread

EXEC_NAME=test_libomdb

SERVER_NAME=temp_server

OBJECTS := main.o

SERVER_OBJECTS := temp_server.o \
	serialization_helper.o

SERVER_SRC = $(SERVER_OBJECTS:.o=.cc)

all: $(EXEC_NAME) $(SERVER_NAME)
	@echo -e "Compilation done!"

$(EXEC_NAME): $(OBJECTS)
	@echo -e "Compiling OMDB test client"
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXEC_NAME)

$(SERVER_NAME): $(SERVER_OBJECTS)
	@echo -e "Compiling OMDB test server"
	$(CC) $(SERVER_OBJECTS) $(SERVER_LDFLAGS) -o $(SERVER_NAME)

temp_server.o: temp_server.cc
	@echo -e "Compiling server source"
	$(CC) $(SERVER_CFLAGS) -c $< -o $@
	
serialization_helper.o: serialization_helper.cc
	@echo -e "Compiling server source"
	$(CC) $(SERVER_CFLAGS) -c $< -o $@

%.o : %.cc
	@echo -e "Compiling source file"
	$(CC) $(CFLAGS) -c $< -o $@
	@echo -e "Done compiling"

clean:
	rm -f $(EXEC_NAME)
	rm -f $(OBJECTS)
	rm -f $(SERVER_OBJECTS)
	rm -f temp_server
