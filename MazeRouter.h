#ifndef MAZE_ROUTER_H
#define MAZE_ROUTER_H

#include <vector>
using std::vector;

#include "Graphic/Point.h"
#include "Net.h"
#include "Block.h"

class MazeRouter
{
  public:

    vector<vector<Block>>& blocks();
    vector<Point>&         pins  ();
    vector<Point>&         path  ();

    void readNet( Net &net );
    void saveNet( Net &net );

    bool route();

  private:

    void findPath ();
    void backTrace();

    vector<vector<Block>> mBlocks;
    vector<Point>         mPins;
    vector<Point>         mPath;
};

#endif
