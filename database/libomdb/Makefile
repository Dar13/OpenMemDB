# Compiler to use
CC=g++

INC_DIR := -I../ \

# Basic flags that will be shared between debug and release configurations
CFLAGS=-Wall -Wextra -g $(INC_DIR) -std=c++11

LDFLAGS=-rdynamic -pthread

LIB_NAME := libomdb.a

SOURCES := ./connection.cc \
	   ./libomdb_result.cc \
	   ./serialization_helper.cc \
	   ./omdb_lib.cc \

OBJECTS = $(SOURCES:.cc=.o)

all: $(SOURCES) $(LIB_NAME)

$(LIB_NAME): $(OBJECTS)
	ar rvs $(LIB_NAME) $(OBJECTS)
	ranlib $(LIB_NAME)

%.o : %.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(LIB_NAME)
	rm $(OBJECTS)
