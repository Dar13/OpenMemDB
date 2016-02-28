CC=g++

INC_DIR := -I../../ \
	   -I../../../ \
	   -I../../../include \

CFLAGS=-std=c++11 $(INC_DIR) -g -Wall -Wextra
LDFLAGS=-pthread

EXEC_NAME=test_libomdb

OBJECTS= ../../../source/util/serialization_helper.o \
                    ../../connection.o \
                    ../../result.o \
                    main.o \

all: $(EXEC_NAME)
	@echo -e "Compilation done!"

$(EXEC_NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(EXEC_NAME)

%.o : %.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC_NAME)
	rm -f $(OBJECTS)
