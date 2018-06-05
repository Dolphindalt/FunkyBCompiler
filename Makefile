CC=g++
CFLAGS=-g -Wall
INCLUDE=-Iinclude/
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXE=a.out

make: $(SOURCES) $(EXE)

$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(INCLUDE)

.cpp.o:
	$(CC) -c $(CFLAGS) $< -o $@ $(INCLUDE)

clean:
	rm -rf *.o $(EXE)