CC=g++

INC_DIR := -I../

CFLAGS := -std=c++11 $(INC_DIR) -g -Wall -Wextra
LDFLAGS := -pthread -L ../ -lomdb

EXEC_NAME=omdb_console
OBJECTS := main.o

all: $(EXEC_NAME)
	@echo -e "Compilation done!"

$(EXEC_NAME): $(OBJECTS)
	@echo -e "Compiling OMDB console application"
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXEC_NAME)

%.o : %.cc
	@echo -e "Compiling source file"
	$(CC) $(CFLAGS) -c $< -o $@
	@echo -e "Done compiling"

clean:
	rm -f $(EXEC_NAME)
	rm -f $(OBJECTS)
