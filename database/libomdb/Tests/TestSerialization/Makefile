CC=g++
CFLAGS=-std=c++11 -I../../ -I../../../include/util -g -Wall -Wextra
LDFLAGS=-pthread

EXEC_NAME=test_serialization

OBJECTS= ../../../source/util/serialization_helper.o \
                    testSerialization.o

all: $(EXEC_NAME)
	@echo -e "Compilation done!"

$(EXEC_NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(EXEC_NAME)

%.o : %.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC_NAME)
	rm -f $(OBJECTS)
