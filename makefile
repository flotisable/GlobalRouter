CPP      := g++
CXXFLAGS := -std=c++98

graphicDir      := Graphic/
componentDir    := Component/
routingGraphDir := RoutingGraph/
gridMapDir      := GridMap/

graphicObj      := ${graphicDir}Point.o ${graphicDir}Rectangle.o
componentObj    := ${componentDir}Block.o ${componentDir}Group.o ${componentDir}/Net.o ${componentDir}Path.o ${componentDir}Pin.o ${componentDir}Symmetry.o
routingGraphObj := ${routingGraphDir}RoutingGraph.o ${routingGraphDir}RoutingRegion.o
gridMapObj      := ${gridMapDir}GridMap.o

all : globalRouter

globalRouter : main.o Router.o MazeRouter.o ${graphicObj} ${componentObj} ${routingGraphObj} ${gridMapObj}
	${CPP} ${CXXFLAGS} -o $@ $^

main.o: main.cpp MazeRouter.h Graphic/Point.h Component/Net.h Component/Pin.h Component/Block.h Graphic/Rectangle.h \
 Component/Path.h GridMap/GridMap.h GridMap/Grid.h GridMap/Edge.h Router.h RoutingGraph/RoutingGraph.h \
 RoutingGraph/RoutingRegion.h Component/Net.h Component/Group.h Component/Symmetry.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

Router.o: Router.cpp Router.h RoutingGraph/RoutingGraph.h RoutingGraph/RoutingRegion.h Component/Block.h \
 Graphic/Point.h Graphic/Rectangle.h Component/Net.h Component/Pin.h Component/Path.h GridMap/GridMap.h GridMap/Grid.h \
 GridMap/Edge.h Component/Group.h Component/Symmetry.h MazeRouter.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

MazeRouter.o: MazeRouter.cpp MazeRouter.h Graphic/Point.h Component/Net.h Component/Pin.h Component/Block.h \
 Graphic/Rectangle.h Component/Path.h GridMap/GridMap.h GridMap/Grid.h GridMap/Edge.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

${graphicObj} :
	${MAKE} -C ${graphicDir}

${componentObj} :
	${MAKE} -C ${componentDir}

${routingGraphObj} :
	${MAKE} -C ${routingGraphDir}

${gridMapObj} :
	${MAKE} -C ${gridMapDir}

clean :
	${MAKE} -C ${graphicDir} clean
	${MAKE} -C ${componentDir} clean
	${MAKE} -C ${routingGraphDir} clean
	${MAKE} -C ${gridMapDir} clean
	rm *.o globalRouter

