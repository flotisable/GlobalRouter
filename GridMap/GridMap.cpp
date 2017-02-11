#include "GridMap.h"
#include <stdexcept>

GridMap::GridMap( int row , int column , int layer )
  : mRow( row ) , mCol( column ) , mLayer( layer )
{
  initGridMap ( row , column , layer );
  connectEdges();
}

GridMap::GridMap( const GridMap &map )
{
  initGridMap( map.row() , map.col() , map.layer() );
  copyGridMap( map );
}

GridMap& GridMap::operator=( const GridMap &map )
{
  copyGridMap( map );

  return *this;
}

void GridMap::initGridMap( int row , int col , int layer )
{
  if( row   <= 0 ) throw std::invalid_argument{ "GridMap row    <= 0" };
  if( col   <= 0 ) throw std::invalid_argument{ "GridMap col    <= 0" };
  if( layer <= 0 ) throw std::invalid_argument{ "GridMap layer  <= 0" };

  grids.resize( row , vector<Grid>{ static_cast<unsigned int>( col ) } );
  edges.resize( ( row - 1 ) * col + ( col - 1 ) * row , Edge{ layer } );
}

void GridMap::connectEdges()
{
  /*  grid and edge example:

      g-g-g
      | | |
      g-g-g
      | | |
      g-g-g

      - is hEdge, | is vEdge

      edge index:

      g 10 g 11 g
      7    8    9
      g  5 g  6 g
      2    3    4
      g  0 g  1 g
  */

  const int hEdges    = mCol - 1;
  const int vEdges    = mCol;
  const int rowEdges  = hEdges + vEdges;

  for( int i = 0 ; i < mRow ; ++i )
     for( int j = 0 ; j < mCol ; ++j )
     {
        const int topIndex    = i * rowEdges + hEdges + j;
        const int bottomIndex = ( i - 1 ) * rowEdges + hEdges + j;
        const int leftIndex   = i * rowEdges + j - 1;
        const int rightIndex  = i * rowEdges + j;

        if( i != mRow - 1 ) grids[i][j].setEdge( &edges[topIndex]    , Grid::top    );
        if( i != 0        ) grids[i][j].setEdge( &edges[bottomIndex] , Grid::bottom );
        if( j != 0        ) grids[i][j].setEdge( &edges[leftIndex]   , Grid::left   );
        if( j != mCol - 1 ) grids[i][j].setEdge( &edges[rightIndex]  , Grid::right  );
     }
}

void GridMap::copyGridMap( const GridMap &map )
{
  mRow    = map.row();
  mCol    = map.col();
  mLayer  = map.layer();
  grids   = map.grids;
  edges   = map.edges;
  connectEdges();
}

void outputGridMapValue( std::ostream &out , const GridMap &map )
{
  for( int i = map.row() - 1 ; i >=0 ; --i )
  {
     for( const Grid &grid : map.grid( i ) )
        switch( grid.value() )
        {
          case Grid::Value::space:    out << "0"; break;
          case Grid::Value::obstacle: out << "1"; break;
          default:                                break;
        }
     out << std::endl;
  }
  out << std::endl;
}
