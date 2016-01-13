# Compiler to use
CC=g++

SRC_DIR=./
EXEC=Connection

# Basic flags that will be shared between debug and release configurations
CFLAGS=-Wall -Wextra -g -std=c++11

LDFLAGS=-rdynamic -pthread

FILES=result.cc connection.cc

SOURCES=$(addprefix $(SRC_DIR), $(FILES))
OBJECTS=$(SOURCES:.cc=.o)


all: $(SOURCES) $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cc.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC)
	rm $(OBJECTS)
