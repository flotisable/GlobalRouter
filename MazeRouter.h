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

    inline void setGridMap  ( const GridMap &map  );
    inline void setGridMap  ( GridMap       &&map );
    inline void setPins     ( const vector<Point> &pins  );
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

    static const Point      nullPoint;
    static constexpr double nullCostDiff  = -1.0;
    const double            wireWidthMin  = 0.46;

    const Block*  getBlock    ( const Point &point );
    bool          findPath    ( const Point &source , const Point &target );
    vector<Path>  backTrace   ( const Point &source , const Point &target );
    void          insertPath  ( const Path  &path );
    void          uniquePaths ();

    void    initSource  ( const Point &source );
    Point   move        ( const Point &point , Direct direction );
    int     getFanin    ( const Point &point );
    bool    gridBlocked ( const Point &point );
    double  getCostDiff ( const Point &point , int layer , Direct direction );
    bool    setGridInfo ( const Point &point , int label , double cost ,
                          Direct direction , int layer );
    Direct  getDirect   ( const Point &p , const Point &movedP );

    void output( const Point &source , const Point &target );

    const Block *sourceBlock  { nullptr };
    const Block *targetBlock  { nullptr };
    int         tag           { 1 };
    int         maxLayer      {};

    Point gridMax;

    GridMap       map{ 0 , 0 , 1 };
    vector<Point> pins;
    vector<Path>  paths;
};

inline void MazeRouter::setGridMap  ( const GridMap &map )          { this->map   = map ;  }
inline void MazeRouter::setGridMap  ( GridMap       &&map )
{ this->map   = std::move( map ); }
inline void MazeRouter::setPins     ( const vector<Point> &pins  )  { this->pins  = pins; }
inline void MazeRouter::setGridMax  ( double x , double y )
{ gridMax = Point( x , y ); }
inline void MazeRouter::setMaxLayer ( int maxLayer ) { this->maxLayer = maxLayer; }

inline void MazeRouter::saveNet( Net &net )
{ for( const Path &path : paths )
     if( !path.path().empty() ) net.paths().push_back( path ); }

#endif
