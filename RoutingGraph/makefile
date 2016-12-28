CPP      := g++
CXXFLAGS := -std=c++11

all : RoutingGraph.o RoutingRegion.o

RoutingGraph.o: RoutingGraph.cpp RoutingGraph.h RoutingRegion.h ../Component/Block.h ../Graphic/Point.h \
 ../Graphic/Rectangle.h ../Graphic/Point.h ../Component/Net.h ../Component/Pin.h ../Component/Path.h \
 ../GridMap/GridMap.h ../GridMap/Grid.h ../GridMap/Edge.h ../Component/Group.h ../Component/Symmetry.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

RoutingRegion.o: RoutingRegion.cpp RoutingRegion.h ../Component/Block.h ../Graphic/Point.h ../Graphic/Rectangle.h \
 ../Component/Net.h ../Component/Pin.h ../Component/Path.h ../GridMap/GridMap.h ../GridMap/Grid.h ../GridMap/Edge.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

clean :
	rm *.o

