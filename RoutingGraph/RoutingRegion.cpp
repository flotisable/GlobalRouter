#include "RoutingRegion.h"

#include <algorithm>
#include <cassert>
using namespace std;

#include "../Component/Pin.h"

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
