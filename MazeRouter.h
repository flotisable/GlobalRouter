#ifndef MAZE_ROUTER_H
#define MAZE_ROUTER_H

#include <vector>
using std::vector;

#include "Graphic/Point.h"
#include "Component/Net.h"
#include "Component/Grid.h"
#include "Component/Path.h"

class Block;

class MazeRouter
{
  public:

    inline MazeRouter();

    inline void setGrids    ( const vector<vector<Grid> > grids );
    inline void setPins     ( const vector<Point>         pins  );
    inline void setGridMax  ( double x , double y );
    inline void setMaxLayer ( int maxLayer );

    inline void saveNet( Net &net );

    bool route();

  private:

    enum Direct
    {
      unknown = -1,
      up,
      down,
      left,
      right,
      directNum
    };

    static const Point  nullPoint;
    static const double nullCostDiff;
    const double        wireWidthMin;

    Point         movePin     ( const Point &source , const Point &target );
    bool          findPath    ( const Point &source , const Point &target );
    vector<Path>  backTrace   ( const Point &source , const Point &target );
    void          insertPath  ( const Path  &path );
    void          uniquePaths ();

    void    initSource  ( const Point &source );
    Point   move        ( const Point &point , Direct direction );
    int     getFanin    ( const Point &point );
    double  getCostDiff ( const Point &movedPoint , int layer , Direct direction );
    Direct  getDirect   ( const Point &p , const Point &movedP );
    void    setGridCost ( Grid &grid , int layer , Direct direction , double costDiff );

    void output( const Point &source , const Point &target );

    const Block *block;
    int         tag;
    int         maxLayer;

    Point gridMax;

    vector<vector<Grid> > mGrids;
    vector<Point>         mPins;
    vector<Path>          mPaths;
};

MazeRouter::MazeRouter() : wireWidthMin( 0.23 ) , block( NULL ) , tag( 1 ) , maxLayer( 0 ) {}

inline void MazeRouter::setGrids    ( const vector<vector<Grid> > grids )
{ mGrids  = grids;  }
inline void MazeRouter::setPins     ( const vector<Point>         pins  ) { mPins   = pins;   }
inline void MazeRouter::setGridMax  ( double x , double y )
{ gridMax = Point( x , y ); }
inline void MazeRouter::setMaxLayer ( int maxLayer ) { this->maxLayer = maxLayer; }

inline void MazeRouter::saveNet( Net &net )
{ for( unsigned int i = 0 ; i < mPaths.size() ; ++i )
     if( !mPaths[i].path().empty() ) net.paths().push_back( mPaths[i] ); }

#endif
