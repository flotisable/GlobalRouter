CPP      := g++
CXXFLAGS := -std=c++03

all : Point.o Rectangle.o

Point.o : Point.cpp Point.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

Rectangle.o : Rectangle.cpp Rectangle.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

clean :
	rm *.o
