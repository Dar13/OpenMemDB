CC=g++

INC_DIR := -I../ \
	   -I../../ \
	   -I../../../ \
	   -I../../../include \
	   -I../../../include/util \

CFLAGS := -std=c++11 $(INC_DIR) -g -Wall -Wextra
LDFLAGS := -pthread -L../ -lomdb_lib.a

EXEC_NAME=test_libomdb

OBJECTS := main.o \ 

all: $(EXEC_NAME)
	@echo -e "Compilation done!"

$(EXEC_NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(EXEC_NAME)

%.o : %.cc
	@echo -e "Compiling source file"
	$(CC) $(CFLAGS) -c $< -o $@
	@echo -e "Done compiling"

clean:
	rm -f $(EXEC_NAME)
	rm -f $(OBJECTS)
