# Compiler to use
CC=g++

SRC_DIR=./
EXEC=test

# Basic flags that will be shared between debug and release configurations
CFLAGS=-Wall -Wextra -g -std=c++11 -I../include/util

LDFLAGS=-rdynamic -pthread

FILES=result.cc connection.cc ../source/util/serialization_helper.cc

SOURCES=$(addprefix $(SRC_DIR), $(FILES))
OBJECTS=$(SOURCES:.cc=.o)


all: $(SOURCES)

test: $(SOURCES) ./Tests/Test_libomdb/main.cc $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) ./Tests/Test_libomdb/main.o -o $@

.cc.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC)
	rm $(OBJECTS)
