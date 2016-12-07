#ifndef MAZE_ROUTER_H
#define MAZE_ROUTER_H

#include <vector>
using std::vector;

#include "Graphic/Point.h"
#include "Component/Net.h"
#include "Component/Grid.h"
#include "Component/Path.h"

class MazeRouter
{
  public:

    inline void setGrids  ( const vector<vector<Grid>>  grids );
    inline void setPins   ( const vector<Point>         pins  );
    inline void setGridMax( double x , double y );

    inline void saveNet( Net &net );

    bool route();

  private:

    enum Direct
    {
      up,
      down,
      left,
      right,
      directNum
    };

    enum MoveDirect
    {
      horizontal,
      vertical,
      unknown
    };

    static const Point nullPoint;

    bool          findPath ( const Point &source , const Point &target );
    vector<Point> backTrace( const Point &source , const Point &target );

    Point move( const Point &point , Direct direction );
    int   getFanin( const Point &point );
    void  setGridCost( Grid &grid , MoveDirect direction , double costDiff );

    int tag = 1;

    Point gridMax;

    vector<vector<Grid>>  mGrids;
    vector<Point>         mPins;
    Path                  mPath;
};

inline void MazeRouter::setGrids  ( const vector<vector<Grid>>  grids ) { mGrids  = grids;  }
inline void MazeRouter::setPins   ( const vector<Point>         pins  ) { mPins   = pins;   }
inline void MazeRouter::setGridMax( double x , double y )
{ gridMax = Point( x , y ); }

inline void MazeRouter::saveNet( Net &net )
{ if( !mPath.path().empty() ) net.paths().push_back( mPath ); }

#endif
