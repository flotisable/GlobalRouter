#include "RoutingRegion.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>

#include "../Component/Pin.h"

const Point RoutingRegion::nullPoint = Point{ -1 , -1 };

GridMap RoutingRegion::gridMap( int layer ) const
{
  assert( mVsplit.size() > 0 && mHsplit.size() > 0 );
  if( layer <= 0 ) throw std::invalid_argument{ "RoutingRegion gridMap layer <= 0" };
  GridMap map( mVsplit.size() - 1 , mHsplit.size() - 1 , layer );

  for( const Block &block : mBlocks )
  {
     const int xMin = getIndex( mHsplit , block.left  () );
     const int xMax = getIndex( mHsplit , block.right () ) - 1;
     const int yMin = getIndex( mVsplit , block.bottom() );
     const int yMax = getIndex( mVsplit , block.top   () ) - 1;

     for( int i = yMin ; i <= yMax ; ++i )
        for( int j = xMin ; j <= xMax ; ++j )
        {
           map.grid( i , j ).setValue( Grid::Value::obstacle );
           map.grid( i , j ).setBlock( &block );
        }
  }

  const double maxH = maxGridWidth();
  const double maxV = maxGridHeight();

  for( int i = 0 ; i < map.row() ; ++i )
     for( int j = 0 ; j < map.col() ; ++j )
     {
        const double hSpace = mHsplit[j+1] - mHsplit[j];
        const double vSpace = mVsplit[i+1] - mVsplit[i];

        for( int k = 0 ; k < layer ; ++k )
        {
           Grid &grid = map.grid( i , j );

           if( grid.edge( Grid::top ) )
             grid.edge( Grid::top    )->setCost( maxH - hSpace , k );
           if( grid.edge( Grid::bottom ) )
             grid.edge( Grid::bottom )->setCost( maxH - hSpace , k );
           if( grid.edge( Grid::left ) )
             grid.edge( Grid::left   )->setCost( maxV - vSpace , k );
           if( grid.edge( Grid::right ) )
             grid.edge( Grid::right  )->setCost( maxV - vSpace , k );
        }
     }
  return map;
}

vector<Point> RoutingRegion::connectedPin( const Net &net ) const
{
  vector<Point> pins;

  for( const Pin &pin : net.pins() )
  {
     if(  ( mHsplit.front() <= pin.x() && pin.x() <= mHsplit.back() ) &&
          ( mVsplit.front() <= pin.y() && pin.y() <= mVsplit.back() ) )
       pins.push_back( map( pin ) );
  }
  return pins;
}

Block* RoutingRegion::getBlock( const string &name )
{
  for( Block &block : blocks() )
     if( block.name() == name ) return &block;

  return nullptr;
}

const Block* RoutingRegion::getBlock( const string &name ) const
{
  return const_cast<RoutingRegion*>( this )->getBlock( name );
}

void RoutingRegion::buildSplit()
{
  using namespace std;

  mHsplit.push_back( left   () );
  mHsplit.push_back( right  () );
  mVsplit.push_back( top    () );
  mVsplit.push_back( bottom () );

  for (const Block &block : blocks())
  {
    mHsplit.push_back( block.left   () );
    mHsplit.push_back( block.right  () );
    mVsplit.push_back( block.top    () );
    mVsplit.push_back( block.bottom () );
  }

  sort( mHsplit.begin() , mHsplit.end() );
  sort( mVsplit.begin() , mVsplit.end() );

  auto it = unique( mHsplit.begin() , mHsplit.end() );

  mHsplit.resize( distance( mHsplit.begin() , it ) );
  mHsplit.shrink_to_fit();

  it = unique( mVsplit.begin() , mVsplit.end() );

  mVsplit.resize( distance( mVsplit.begin() , it ) );
  mVsplit.shrink_to_fit();
}

double RoutingRegion::maxGridWidth() const
{
  return maxGridSide( mHsplit );
}

double RoutingRegion::maxGridHeight() const
{
  return maxGridSide( mVsplit );
}

bool RoutingRegion::netConnected( Net &net ) const
{
  for( const Pin &pin : net.pins() )
     if(  ( mHsplit.front() <= pin.x() && pin.x() <= mHsplit.back() ) &&
          ( mVsplit.front() <= pin.y() && pin.y() <= mVsplit.back() ) )
       return true;
  return false;
}

int RoutingRegion::mapX( double x ) const
{
  return mapArray( mHsplit , x );
}

int RoutingRegion::mapY( double y ) const
{
  return mapArray( mVsplit , y );
}

Point RoutingRegion::map( const Point &point ) const
{
  return map( point.x() , point.y() );
}

Point RoutingRegion::map( double x, double y ) const
{
  if( ( mHsplit.front() <= x && x <= mHsplit.back() ) &&
      ( mVsplit.front() <= y && y <= mVsplit.back() ) )
    return Point{ static_cast<double>( mapX( x ) ) , static_cast<double>( mapY( y ) ) };
  return nullPoint;
}


int getIndex( const vector<double> &array , double value )
{
  for( int i = 0 ; i < static_cast<int>( array.size() ) ; ++i )
     if( array[i] == value ) return i;
  return -1;
}

double maxGridSide( const vector<double> &array )
{
  double maxSide = 0;

  for( unsigned int i = 0 ; i < array.size() - 1 ; ++i )
     maxSide = std::max( maxSide , array[i+1] - array[i] );

  return maxSide;
}

int mapArray( const vector<double>& array , double value )
{
  for( unsigned int i = 0 ; i < array.size() ; ++i )
     if( array[i] > value ) return --i;

  return -1;
}
