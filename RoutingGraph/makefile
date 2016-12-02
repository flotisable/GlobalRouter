CPP      := g++
CXXFLAGS := -std=c++11

all : RoutingGraph.o RoutingRegion.o

RoutingGraph.o : RoutingGraph.cpp RoutingGraph.h RoutingRegion.h ../Component/Grid.h ../Component/Net.h ../Component/Group.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

RoutingRegion.o : RoutingRegion.cpp RoutingRegion.h ../Component/Pin.h ../Component/Block.h ../Component/Net.h ../Component/Grid.h ../Graphic/Point.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

clean :
	rm *.o
