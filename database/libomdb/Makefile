# Compiler to use
CC=g++

INC_DIR := -I../ \
	   -I../include \
	   -I../include/util \

# Basic flags that will be shared between debug and release configurations
CFLAGS=-Wall -Wextra -g $(INC_DIR) -std=c++11 -I../include/util

LDFLAGS=-rdynamic -pthread

LIB_NAME := omdb_lib.a

SOURCES := ./connection.cc \
	   ./result.cc \
	   ../source/util/serialization_helper.cc \
	   ./omdb_lib.cc \

OBJECTS = $(SOURCES:.cc=.o)

all: $(SOURCES) $(LIB_NAME)

$(LIB_NAME): $(OBJECTS)
	ar rvs omdb_lib.a $(OBJECTS)

%.o : %.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(LIB_NAME)
	rm $(OBJECTS)
