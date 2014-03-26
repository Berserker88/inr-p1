CC     = g++
CFLAGS = 
LFLAGS = 
OPTSC  = -Wall 
OPTSL  =

.cpp.o:
	$(CC) -c $(OPTSC) $(CFLAGS) $<

SRC    = P1.cpp \
	 tokens.cpp

OBJ    = $(SRC:.cpp=.o)

TARGET = P1


$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(OPTSL) $(LFLAGS)


clean:
	rm -f $(OBJ) $(TARGET)

tokens.cpp: tokens.hpp