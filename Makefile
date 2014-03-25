CFLAGS =-Wall -std=c++11
LIBS =

% : %.cpp
	g++ $(CFLAGS) -o $@ $< $(LIBS)
clean:
	rm -f $(OBJ) $(TARGET)