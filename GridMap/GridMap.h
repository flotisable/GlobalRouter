#ifndef GRIDMAP_H
#define GRIDMAP_H

#include <vector>
#include <ostream>
using std::vector;

#include "Grid.h"
#include "Edge.h"

class GridMap
{
  public:

    GridMap( int row , int column , int layer );
    GridMap( const GridMap  &map  );

    inline int row   () const;
    inline int col   () const;
    inline int layer () const;

    inline Grid&                grid( int row , int column );
    inline const Grid&          grid( int row , int column ) const;
    inline vector<Grid>&        grid( int row );
    inline const vector<Grid>&  grid( int row ) const;

    GridMap& operator=( const GridMap &map  );

  private:

    void initGridMap( int row , int col , int layer );
    void connectEdges();
    void copyGridMap( const GridMap &map );

    int mRow;
    int mCol;
    int mLayer;

    vector<vector<Grid> > grids;
    vector<Edge>          edges;
};

// GridMap non-member functions
void outputGridMapValue( std::ostream &out , const GridMap &map );
// end GridMap non-member functions

// GridMap inline member functions
inline int GridMap::row   () const { return mRow;   }
inline int GridMap::col   () const { return mCol;   }
inline int GridMap::layer () const { return mLayer; }

inline Grid&                GridMap::grid( int row , int column )
{ return grids[row][column]; }
inline const Grid&          GridMap::grid( int row , int column ) const
{ return grids[row][column]; }
inline vector<Grid>&        GridMap::grid( int row )        { return grids[row]; }
inline const vector<Grid>&  GridMap::grid( int row ) const  { return grids[row]; }
// end GridMap inline member functions

#endif // GRIDMAP_H
