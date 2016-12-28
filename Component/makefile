CPP      := g++
CXXFLAGS := -std=c++11

all : Block.o Group.o Net.o Path.o Pin.o Symmetry.o

Block.o: Block.cpp Block.h ../Graphic/Point.h ../Graphic/Rectangle.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

Group.o: Group.cpp Group.h ../RoutingGraph/RoutingRegion.h Block.h ../Graphic/Point.h ../Graphic/Rectangle.h \
 Net.h Pin.h Path.h ../GridMap/GridMap.h ../GridMap/Grid.h ../GridMap/Edge.h Symmetry.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

Net.o: Net.cpp Net.h Pin.h ../Graphic/Point.h Block.h ../Graphic/Rectangle.h Path.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

Path.o: Path.cpp Path.h ../Graphic/Point.h ../RoutingGraph/RoutingGraph.h ../RoutingGraph/RoutingRegion.h Block.h \
 ../Graphic/Rectangle.h Net.h Pin.h ../GridMap/GridMap.h ../GridMap/Grid.h ../GridMap/Edge.h Group.h Symmetry.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

Pin.o: Pin.cpp Pin.h ../Graphic/Point.h Block.h ../Graphic/Rectangle.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

Symmetry.o: Symmetry.cpp Symmetry.h Block.h ../Graphic/Point.h ../Graphic/Rectangle.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

clean :
	rm *.o
