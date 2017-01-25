#include "RoutingRegion.h"

#include <algorithm>
#include <cassert>
using namespace std;

#include "../Component/Pin.h"

const Point RoutingRegion::nullPoint = Point{ -1 , -1 };

GridMap RoutingRegion::gridMap( int layer ) const
{
  assert( mVsplit.size() > 0 && mHsplit.size() > 0 );
  GridMap map( mVsplit.size() - 1 , mHsplit.size() - 1 , layer );

  for( const Block &block : mBlocks )
  {
     int xMin = getIndex( mHsplit , block.left  () );
     int xMax = getIndex( mHsplit , block.right () ) - 1;
     int yMin = getIndex( mVsplit , block.bottom() );
     int yMax = getIndex( mVsplit , block.top   () ) - 1;

     for( int i = yMin ; i <= yMax ; ++i )
        for( int j = xMin ; j <= xMax ; ++j )
        {
           map.grid( i , j ).setValue( Grid::Value::obstacle );
           map.grid( i , j ).setBlock( &block );
        }
  }

  double maxH = maxGridWidth();
  double maxV = maxGridHeight();

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
  for( const Block &block : blocks() )
     if( block.name() == name ) return &block;

  return nullptr;
}

double RoutingRegion::maxGridWidth() const
{
  double maxWidth = 0;

  for( unsigned int i = 0 ; i < hsplit().size() - 1 ; ++i )
     maxWidth = max( maxWidth , hsplit()[i+1] - hsplit()[i] );

  return maxWidth;
}

double RoutingRegion::maxGridHeight() const
{
  double maxHeight = 0;

  for( unsigned int i = 0 ; i < vsplit().size() - 1 ; ++i )
     maxHeight = max( maxHeight , vsplit()[i+1] - vsplit()[i] );

  return maxHeight;
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
  for( unsigned i = 0 ; i < mHsplit.size() ; ++i )
     if( mHsplit[i] > x ) return --i;
  return -1;
}

int RoutingRegion::mapY( double y ) const
{
  for( unsigned i = 0 ; i < mVsplit.size() ; ++i )
     if( mVsplit[i] > y )
       return --i;
  return -1;
}

Point RoutingRegion::map( const Point &point ) const
{
  if( ( mHsplit.front() <= point.x() && point.x() <= mHsplit.back() ) &&
      ( mVsplit.front() <= point.y() && point.y() <= mVsplit.back() ) )
    return Point{ static_cast<double>( mapX( point.x() ) ) , static_cast<double>( mapY( point.y() ) ) };
  return nullPoint;
}

Point RoutingRegion::map( double x, double y ) const
{
  return map( Point{ x , y } );
}


int RoutingRegion::getIndex( const vector<double> &array , double value ) const
{
  for( int i = 0 ; i < static_cast<int>( array.size() ) ; ++i )
     if( array[i] == value ) return i;
  return -1;
}
