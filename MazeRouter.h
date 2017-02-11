#ifndef MAZE_ROUTER_H
#define MAZE_ROUTER_H

#include <vector>
#include <utility>
using std::vector;

#include "Graphic/Point.h"
#include "Component/Net.h"
#include "Component/Path.h"
#include "GridMap/GridMap.h"

class Block;

class MazeRouter
{
  public:

    typedef Grid::CostType CostType;

    class BacktraceError{}; // for exception

    enum Direct
    {
      unknown   = -1,
      up,
      down,
      left,
      right,
      directNum
    };

    inline MazeRouter();

    inline const vector<Path>& paths();

    inline void setGridMap  ( const GridMap &map  );
    inline void setPins     ( const vector<Point> &pins  );
    inline void setGridMax  ( double x , double y );
    inline void setMaxLayer ( int maxLayer );

    bool route();

  private:

    static const Point  nullPoint;
    static const double nullCostDiff;
    const double        wireWidthMin;

    const Block*  getBlock    ( const Point &point );
    bool          findPath    ( const Point &source , const Point &target );
    vector<Path>  backTrace   ( const Point &source , const Point &target );
    void          insertPath  ( const Path  &path );

    void      initSource  ( const Point &source );
    Point     move        ( const Point &point , Direct direction );
    int       getFanin    ( const Point &point );
    bool      gridBlocked ( const Point &point );
    CostType  getCostDiff ( const Point &point , int layer , Direct direction );
    bool      setGridInfo ( const Point &point , int label , double cost ,
                            Direct direction , int layer );

    const Block *sourceBlock;
    const Block *targetBlock;
    int         tag;
    int         maxLayer;
    Point       gridMax;
    GridMap     map;

    vector<Point> pins;
    vector<Path>  mPaths;
};

// MazeRouter non-member functions
void                uniquePaths ( vector<Path> &paths );
MazeRouter::Direct  getDirect   ( const Point &p      , const Point &movedP );
void                output      ( const Point &source , const Point &target , const GridMap &map );

MazeRouter::Direct& operator++( MazeRouter::Direct &direct );
// end MazeRouter non-member functions

// MazeRouter inline member functions
inline MazeRouter::MazeRouter()
  : wireWidthMin( 0.46 ) , sourceBlock( NULL ) , targetBlock( NULL ) , tag( 1 ) , maxLayer( 0 ) , map( 1 , 1 , 1 ) {}

inline const vector<Path>& MazeRouter::paths() { return mPaths; }

inline void MazeRouter::setGridMap  ( const GridMap &map )          { this->map   = map ;  }
inline void MazeRouter::setPins     ( const vector<Point> &pins  )  { this->pins  = pins; }
inline void MazeRouter::setGridMax  ( double x , double y )
{ gridMax = Point( x , y ); }
inline void MazeRouter::setMaxLayer ( int maxLayer ) { this->maxLayer = maxLayer; }
// end MazeRouter inline member functions

#endif
