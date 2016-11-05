#ifndef MAZE_ROUTER_H
#define MAZE_ROUTER_H

#include <vector>
using std::vector;

#include "Graphic/Point.h"
#include "Net.h"
#include "Grid.h"

class MazeRouter
{
  public:

    vector<vector<Grid>>& grids ();
    vector<Point>&        pins  ();
    vector<Point>&        path  ();

    void readNet( Net &net );
    void saveNet( Net &net );

    bool route();

  private:

    void findPath ();
    void backTrace();

    vector<vector<Grid>> mGrids;
    vector<Point>        mPins;
    vector<Point>        mPath;
};

#endif
