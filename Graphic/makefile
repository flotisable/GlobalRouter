CPP      := g++
CXXFLAGS := -std=c++11

all : Point.o Rectangle.o

Point.o : Point.cpp Point.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

Rectangle.o: Rectangle.cpp Rectangle.h Point.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

clean :
	rm *.o
