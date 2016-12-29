#include "GridMap.h"
#include <iostream>

GridMap::GridMap( int row , int column , int layer )
  : mRow( row ) , mCol( column ) , mLayer( layer )
{
  grids.resize( row , vector<Grid>( column ) );
  edges.resize( ( row - 1 ) * column + ( column - 1 ) * row , Edge( layer ) );

  connectEdges();
}

void GridMap::connectEdges()
{
  const int hEdges    = mCol - 1;
  const int vEdges    = mCol;
  const int rowEdges  = hEdges + vEdges;

  for( int i = 0 ; i < mRow ; ++i )
     for( int j = 0 ; j < mCol ; ++j )
     {
        int topIndex    = i * rowEdges + hEdges + j;
        int bottomIndex = ( i - 1 ) * rowEdges + hEdges + j;
        int leftIndex   = i * rowEdges + j - 1;
        int rightIndex  = i * rowEdges + j;

        if( i != mRow - 1 ) grids[i][j].setEdge( &edges[topIndex]    , Grid::top     );
        if( i != 0        ) grids[i][j].setEdge( &edges[bottomIndex] , Grid::bottom  );
        if( j != 0        ) grids[i][j].setEdge( &edges[leftIndex]   , Grid::left    );
        if( j != mCol - 1 ) grids[i][j].setEdge( &edges[rightIndex]  , Grid::right   );
     }
}
