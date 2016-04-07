# Compiler to use
CC=g++

# NAWK-compatible AWK
NAWK=gawk

TERVEL_DIR=../../Tervel
INC_DIR=../include
SRC_DIR=../source
EXEC=tests

TOOLS_DIR=../tools

LEMON_GRAM_LOC=$(realpath ./include/sql/parser/parse.y)

# Basic flags that will be shared between debug and release configurations
CFLAGS=-Wall -Wextra -g -std=c++11 -I$(INC_DIR) -I$(TERVEL_DIR)

LDFLAGS=-L$(TERVEL_DIR) -rdynamic -pthread -std=c++11

SOURCES=$(shell find $(SRC_DIR) -name "*.cc" ! -name "main.cc")

SOURCES += $(shell find $(TERVEL_DIR)/tervel/util/ -name "*.cc")
SOURCES += $(shell find -name "*.cc")

OBJECTS=$(SOURCES:.cc=.o)

all: $(SOURCES) $(EXEC)

release: CFLAGS += -O2
release: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cc.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC)
	rm -f $(OBJECTS)

lemon:
	make -C $(TOOLS_DIR)/lemon
	cd $(TOOLS_DIR)/lemon && ./lemon -p $(LEMON_GRAM_LOC)
	mv $(INC_DIR)/sql/parser/parse.h $(INC_DIR)/sql/parser/parse.h.tmp
	mv $(INC_DIR)/sql/parser/parse.c $(SRC_DIR)/sql/parser/parse.c
	$(NAWK)	-f $(TOOLS_DIR)/lemon/addopcodes.awk $(INC_DIR)/sql/parser/parse.h.tmp >$(INC_DIR)/sql/parser/parse.h
	rm $(INC_DIR)/sql/parser/parse.h.tmp
