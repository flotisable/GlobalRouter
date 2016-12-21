CPP      := g++
CXXFLAGS := -std=c++11
VPATH    := Graphic Component RoutingGraph

graphicDir      := Graphic/
componentDir    := Component/
routingGraphDir := RoutingGraph/

graphicObj      := ${graphicDir}Point.o ${graphicDir}Rectangle.o
componentObj    := ${componentDir}Block.o ${componentDir}Group.o ${componentDir}/Net.o ${componentDir}Path.o ${componentDir}Pin.o ${componentDir}Symmetry.o
routingGraphObj := ${routingGraphDir}RoutingGraph.o ${routingGraphDir}RoutingRegion.o

all : globalRouter

globalRouter : main.o Router.o MazeRouter.o ${graphicObj} ${componentObj} ${routingGraphObj}
	${CPP} ${CXXFLAGS} -o $@ $^

main.o : main.cpp Router.h MazeRouter.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

Router.o : Router.cpp Router.h Component/Block.h MazeRouter.h RoutingGraph/RoutingGraph.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

MazeRouter.o : MazeRouter.cpp MazeRouter.h Graphic/Point.h Component/Net.h Component/Grid.h Component/Path.h
	${CPP} ${CXXFLAGS} -o $@ -c $<

${graphicObj} :
	${MAKE} -C ${graphicDir}

${componentObj} :
	${MAKE} -C ${componentDir}

${routnigGraphObj} :
	${MAKE} -C ${routingGraphDir}

clean :
	${MAKE} -C ${graphicDir} clean
	${MAKE} -C ${componentDir} clean
	${MAKE} -C ${routingGraphDir} clean
	rm *.o globalRouter
