CPP      := g++
CXXFLAGS := -std=c++11

all: GridMap.o

GridMap.o: GridMap.cpp GridMap.h Grid.h Edge.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

clean:
	rm *.o
