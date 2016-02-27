CC=g++

CFLAGS=-std=c++11 -I../../../ -I../../../include/util -I../../ -g -Wall -Wextra
LDFLAGS=-pthread

EXEC_NAME=test_libomdb
# What goes under here? Every .cc file that needs to be compiled, but with a .o suffix
# The “\” does a new-line, don’t worry about it: Alright
OBJECTS= ../../../source/util/serialization_helper.o \
                    ../../connection.o \
                    ../../result.o \
                    main.o \

all: $(EXEC_NAME)
	@echo -e "Compilation done!"

$(EXEC_NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(EXEC_NAME)

%.o : %.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC_NAME)
	rm -f $(OBJECTS)
