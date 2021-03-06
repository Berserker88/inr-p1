# the compiler to use.
CC=g++
# options to pass to the compiler.
CFLAGS=-c -Wall #-O3 -funroll-loops -funroll-all-loops
# options to pass to the linker
LDFLAGS=-lm
# the soure files
SOURCES=P1.cpp Document.cpp Index.cpp IndexDict.cpp Posting.cpp tokens.cpp QueryParser.cpp
# the object files
OBJECTS=$(SOURCES:.c=.o)
# name of the executable
EXECUTABLE=P1

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *o $(EXECUTABLE) 