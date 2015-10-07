# Compiler to use
CC=g++

TERVEL_DIR=../Tervel
INC_DIR=./include/
SRC_DIR=./source/
EXEC=openmemdb

# Basic flags that will be shared between debug and release configurations
CFLAGS=-Wall -Wextra -g -std=c++11 -I$(INC_DIR) -I$(TERVEL_DIR)

LDFLAGS=-L$(TERVEL_DIR) -rdynamic -pthread -std=c++11

FILES=main.cc

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

