#include "MazeRouter.h"

#include <queue>
#include <climits>
#include <iostream>
using namespace std;

bool MazeRouter::route()
{
  Point source = mPins.front();

  for( const Point &pin : mPins )
  {
     Point target = pin;

     if( source == target ) continue;

     if( findPath( source , target ) )  backTrace( source , target );
     else                               return false;

     ++tag;
     source = target;
  }
  return true;
}


bool MazeRouter::findPath( const Point &source , const Point &target )
{
  int           fanin       = 0;
  int           arrivedNum  = 0;
  int           label       = 0;
  queue<Point>  box;
  
  if( target.x() + 1  != mGrids[0].size() &&
      mGrids[target.y()][target.x()+1].label() != Grid::OBSTACLE ) ++fanin;
  if( target.x() != 0 &&
      mGrids[target.y()][target.x()-1].label() != Grid::OBSTACLE ) ++fanin;
  if( target.y() + 1 != mGrids.size() &&
      mGrids[target.y()+1][target.x()].label() != Grid::OBSTACLE ) ++fanin;
  if( target.y() != 0 &&
      mGrids[target.y()-1][target.x()].label() != Grid::OBSTACLE ) ++fanin;

  mGrids[source.y()][source.x()].setTag   ( tag );
  mGrids[source.y()][source.x()].setLabel ( label );
  box.push( source );
  ++label;
  
  while( !box.empty() )
  {
    const unsigned int boxNum = box.size();

    for( unsigned int i = 0 ; i < boxNum ; ++i , box.pop() )
    {
      for( int direct = 0 ; direct < directNum ; ++direct )
      {
         unsigned int x = box.front().x();
         unsigned int y = box.front().y();
         int cost       = mGrids[y][x].cost();
    
         switch( direct )
         {
           case up:
         
             if( y + 1 == mGrids.size() ) continue;
             ++y;
             break;
         
           case down:
         
             if( y == 0 ) continue;
             --y;
             break;
         
           case left:
         
             if( x + 1 == mGrids[0].size() ) continue;
             ++x;
             break;
         
           case right:
         
             if( x == 0 ) continue;
             --x;
             break;
         
           default: break;
         }
       
         if( x == target.x() && y == target.y() )
         {
           ++arrivedNum;
           if( arrivedNum == fanin )
           {
              for( int i = static_cast<int>( mGrids.size() ) - 1 ; i >= 0 ; --i )
              {
                 for( const Grid &grid : mGrids[i] )
                    cout << grid.tag();
                 cout << endl;
              }
            return true;
           }
           continue;
         }

         if( mGrids[y][x].label() == Grid::OBSTACLE ) continue;

         if( mGrids[y][x].tag() == tag )
         {
           cost +=  ( direct == up || direct == down ) ?
                    mGrids[y][x].costY() : mGrids[y][x].costX();
         
           if( cost < mGrids[y][x].cost() )
           {
             mGrids[y][x].setCost( cost );
             box.push( Point( x , y ) );
           }
         }
         else
         {
           mGrids[y][x].setTag   ( tag );
           mGrids[y][x].setLabel ( label );
           mGrids[y][x].setCost  ( cost + ( direct == up || direct == down ) ?
                                  mGrids[y][x].costY() : mGrids[y][x].costX() );
           box.push( Point( x , y ) );
         }
      }
    }
    ++label;
  }
  return false;
}

void MazeRouter::backTrace( const Point &source , const Point &target )
{
  enum MoveDirect
  {
    horizontal,
    vertical,
    unknown
  };

  constexpr double wireWidth = 0.23;

  int x                 = target.x();
  int y                 = target.y();
  int cost              = INT_MAX;
  int label             = INT_MAX;
  MoveDirect direction  = unknown;

  mPath.clear();
  mPath.push_back( target );
  
  while( x != source.x() || y != source.y() )
  {
    int xNext;
    int yNext;
  
    for( int direct = 0 ; direct < directNum ; ++direct )
    {
       unsigned int xT = x;
       unsigned int yT = y;
    
       switch( direct )
       {
         case up:

           if( yT + 1 == mGrids.size() ) continue;
           ++yT;
           break;

         case down:

           if( yT == 0 ) continue;
           --yT;
           break;

         case left:

           if( xT + 1 == mGrids[0].size() ) continue;
           ++xT;
           break;

         case right:

           if( xT == 0 ) continue;
           --xT;
           break;

         default: break;
       }
       if( mGrids[yT][xT].tag() != tag ) continue;

       if( mGrids[yT][xT].cost() <= cost && mGrids[yT][xT].label() < label )
       {
         xNext  = xT;
         yNext  = yT;
         cost   = mGrids[yT][xT].cost();
         label  = mGrids[yT][xT].label();
       }
    }
    if( direction == unknown )
    {
      if      ( x != xNext ) direction = horizontal;
      else if ( y != yNext ) direction = vertical;
    }
    else
    {
      if( x != xNext && direction == vertical )
      {
        direction = horizontal;
        mPath.push_back( Point( x , y ) );
      }
      else if( y != yNext && direction == horizontal )
      {
        direction = vertical;
        mPath.push_back( Point( x , y ) );
      }
    }
    if      ( direction == horizontal )
      mGrids[y][x].setCostX( mGrids[y][x].costX() + wireWidth );
    else if ( direction == vertical )
      mGrids[y][x].setCostY( mGrids[y][x].costY() + wireWidth );
    x = xNext;
    y = yNext;
  }
  if      ( direction == horizontal )
    mGrids[y][x].setCostX( mGrids[y][x].costX() + wireWidth );
  else if ( direction == vertical )
    mGrids[y][x].setCostY( mGrids[y][x].costY() + wireWidth );
  mPath.push_back( source );
}
