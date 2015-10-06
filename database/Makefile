# Compiler to use
CC=g++

TERVEL_DIR=../Tervel
INC_DIR=./include/
SRC_DIR=./source/
EXEC=openmemdb

# Basic flags that will be shared between debug and release configurations
CFLAGS=-Wall -Wextra -g -std=c++11 -I$(INC_DIR) -I$(TERVEL_DIR)

LDFLAGS=-L$(TERVEL_DIR) -lpthread -rdynamic

FILES=main.cc

SOURCES=$(addprefix $(SRC_DIR), $(FILES))
OBJECTS=$(SOURCES:.cc=.o)


all: $(SOURCES) $(EXEC) 

$(EXEC): $(OBJECTS)
	cd ../Tervel/tervel/tests && $(MAKE) allTervel
	$(eval TERVEL_OBJECTS:=$(shell find $(TERVEL_DIR)/tervel/util -name *.o))
	$(CC) $(LDFLAGS) $(TERVEL_OBJECTS) $(OBJECTS) -o $@

.cc.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC) 
	rm -f $(SRC_DIR)*.o
