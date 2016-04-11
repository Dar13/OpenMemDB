#Compiler to use
CC=gcc

SRC_DIR=.

SOURCES=$(shell find $(SRC_DIR) -name "*.cc")

OBJECTS=$(SOURCES:.c=.o)

all:
	$(SOURCES) $(EXEC) 

$(EXEC): $(OBJECTS)
	$(CC) $(LDLFLAGS) -o $@

clean:
	rm -f $(EXEC)
	rm -f $(OBJECTS)
