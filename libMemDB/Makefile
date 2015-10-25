# Compiler to use
CC=g++

SRC_DIR=./
EXEC=libmem

# Basic flags that will be shared between debug and release configurations
CFLAGS=-Wall -Wextra -g -std=c++11

LDFLAGS=-L$(TERVEL_DIR) -rdynamic -pthread -std=c++11

FILES=resultset.h resultset.cc connector.h connector.cc libmem.h libmem.cc Tests/libmem_test.cc

SOURCES=$(addprefix $(SRC_DIR), $(FILES))
SOURCES += $(shell find $(TERVEL_DIR)/tervel/util/ -name *.cc)
OBJECTS=$(SOURCES:.cc=.o)


all: $(SOURCES) $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cc.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC)
	rm $(OBJECTS)