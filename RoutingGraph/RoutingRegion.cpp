#include "RoutingRegion.h"

#include <algorithm>
#include <cassert>
using namespace std;

#include "../Component/Pin.h"

vector<vector<Grid>> RoutingRegion::gridMap( int layer ) const
{
  assert( mVsplit.size() > 0 && mHsplit.size() > 0 );
  vector<vector<Grid>> grids( mVsplit.size() - 1 ,
                              vector<Grid>( mHsplit.size() - 1 , Grid( layer ) ) );

  for( const Block &block : mBlocks )
  {
     int xMin = getIndex( mHsplit , block.left  () );
     int xMax = getIndex( mHsplit , block.right () ) - 1;
     int yMin = getIndex( mVsplit , block.bottom() );
     int yMax = getIndex( mVsplit , block.top   () ) - 1;

     for( int i = yMin ; i <= yMax ; ++i )
        for( int j = xMin ; j <= xMax ; ++j )
           grids[i][j].setLabel( Grid::OBSTACLE );
  }

  double maxH = maxGridWidth();
  double maxV = maxGridHeight();

  for( unsigned int i = 0 ; i < grids.size() ; ++i )
     for( unsigned int j = 0 ; j < grids[0].size() ; ++j )
     {
        for( int k = 0 ; k < layer ; ++k )
        {
           grids[i][j].setCostTop    ( maxH - ( mHsplit[j+1] - mHsplit[j] ) , k );
           grids[i][j].setCostBottom ( maxH - ( mHsplit[j+1] - mHsplit[j] ) , k );
           grids[i][j].setCostLeft   ( maxV - ( mVsplit[i+1] - mVsplit[i] ) , k );
           grids[i][j].setCostRight  ( maxV - ( mVsplit[i+1] - mVsplit[i] ) , k );
        }
     }

  return grids;
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

vector<Point> RoutingRegion::connectedPin( const Net &net ) const
{
  vector<Point> pins;

  for( const Pin &pin : net.pins() )
  {
     if(  ( mHsplit.front() <= pin.x() && pin.x() <= mHsplit.back() ) &&
          ( mVsplit.front() <= pin.y() && pin.y() <= mVsplit.back() ) )
     {
       unsigned int x;
       unsigned int y;

       for( x = 0 ; x < mHsplit.size() ; ++x )
          if( mHsplit[x] >= pin.x() )
          {
            --x;
            break;
          }
       for( y = 0 ; y < mVsplit.size() ; ++y )
          if( mVsplit[y] >= pin.y() )
          {
            --y;
            break;
          }
       pins.push_back( Point( x , y ) );
     }
  }
  return pins;
}

bool RoutingRegion::netConnected( Net &net ) const
{
  for( const Pin &pin : net.pins() )
     if(  ( mHsplit.front() <= pin.x() && pin.x() <= mHsplit.back() ) &&
          ( mVsplit.front() <= pin.y() && pin.y() <= mVsplit.back() ) )
       return true;
  return false;
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


int RoutingRegion::getIndex( const vector<double> &array , double value ) const
{
  for( int i = 0 ; i < static_cast<int>( array.size() ) ; ++i )
     if( array[i] == value ) return i;
  return -1;
}
