#include "MazeRouter.h"

#include <queue>
#include <climits>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cfloat>
using namespace std;

const Point MazeRouter::nullPoint = Point( -1 , -1 );

bool MazeRouter::route()
{
  Point source = mPins.front();

  mPath.path().clear();

  for( const Point &pin : mPins )
  {
     Point target = pin;

     cout << pin << endl;

     if( source == target ) continue;

     if( findPath( source , target ) )
     {
       vector<Point> path = backTrace( source , target );

       mPath.path().insert( mPath.path().begin() , path.begin() , path.end() );
     }
     else return false;

     ++tag;
     source = target;
  }

  auto it = unique( mPath.path().begin() , mPath.path().end() );

  mPath.path().resize( distance( mPath.path().begin() , it ) );
  
  cout << endl;
  return true;
}


bool MazeRouter::findPath( const Point &source , const Point &target )
{
  int           fanin       = getFanin( target );
  int           arrivedNum  = 0;
  int           label       = 0;
  queue<Point>  box;

  Grid &s = mGrids[source.y()][source.x()];

  s.setTag( tag );
  if( s.label() != Grid::OBSTACLE ) s.setLabel( label );
  s.setCost( 0 );
  box.push( source );
  ++label;
  
  while( !box.empty() )
  {
    const unsigned int boxNum = box.size();

    for( unsigned int i = 0 ; i < boxNum ; ++i , box.pop() )
    {
      for( int direct = 0 ; direct < directNum ; ++direct )
      {
         Point  p     = box.front();
         double cost  = mGrids[p.y()][p.x()].cost();

         p = move( p , static_cast<Direct>( direct ) );

         if( p == nullPoint ) continue;

         if( p == target )
         {
           if( ++arrivedNum == fanin )
           {
             cout << target << source << endl;
             for( int i = static_cast<int>( mGrids.size() ) - 1 ; i >= 0 ; --i )
             {
                for( const Grid &grid : mGrids[i] )
                   cout << setw( 2 ) << grid.tag();
                cout << " ";
                for( const Grid &grid : mGrids[i] )
                   cout << setw( 2 ) << grid.label();
                cout << " ";
                auto precision = cout.precision();
                for( const Grid &grid : mGrids[i] )
                   cout << setw( precision ) << grid.cost();
                cout << endl;
             }
             cout << endl;
             return true;
           }
           continue;
         }

         Grid &grid = mGrids[p.y()][p.x()];

         if( grid.label() == Grid::OBSTACLE ) continue;

         if( direct == up || direct == down )
         {
           if( grid.costX() >= gridMax.x() ) continue;
           cost += grid.costX();
         }
         else
         {
           if( grid.costY() >= gridMax.y() ) continue;
           cost += grid.costY();
         }

         if( grid.tag() != tag )
         {
           grid.setTag  ( tag );
           grid.setLabel( label );
           grid.setCost ( cost );
           box.push( p );

         }
         else if( cost < grid.cost() )
         {
           grid.setCost( cost );
           box.push( p );
         }
      }
    }
    ++label;
  }
  return false;
}

vector<Point> MazeRouter::backTrace( const Point &source , const Point &target )
{
  constexpr double wireWidth = 0.23;

  Point         p         = target;
  double        cost      = DBL_MAX;
  int           label     = INT_MAX;
  MoveDirect    direction = unknown;
  vector<Point> path;

  path.push_back( target );
  
  while( p != source )
  {
    Point pNext;

    // move
    for( int direct = 0 ; direct < directNum ; ++direct )
    {
       Point pT = move( p , static_cast<Direct>( direct ) );

       if( pT == nullPoint ) continue;

       Grid &grid = mGrids[pT.y()][pT.x()];

       if( grid.tag() != tag ) continue;

       if( ( direct == up || direct == down ) )
       {
         if( grid.costX() >= gridMax.x() ) continue;
       }
       else
       {
         if( grid.costY() >= gridMax.y() ) continue;
       }

       if( grid.label() < label && grid.cost() <= cost )
       {
         pNext  = pT;
         cost   = grid.cost();
       }
    }
    // end move

    // set direction
    switch( direction )
    {
      case unknown:

        if      ( p.x() != pNext.x() ) direction = horizontal;
        else if ( p.y() != pNext.y() ) direction = vertical;
        break;

      case vertical:

        if( p.x() != pNext.x() )
        {
          direction = horizontal;
          path.push_back( p );
        }
        break;

      case horizontal:

        if( p.y() != pNext.y() )
        {
          direction = vertical;
          path.push_back( p );
        }
        break;
    }
    // end set direction

    Grid &grid = mGrids[p.y()][p.x()];

    setGridCost( grid , direction , wireWidth );
    p     = pNext;
    label = grid.label();
  }
  setGridCost( mGrids[p.y()][p.x()] , direction , wireWidth );
  path.push_back( source );

  cout << "trace success\n";

  return path;
}

Point MazeRouter::move( const Point &point, MazeRouter::Direct direction )
{
  switch( direction )
  {
    case up:

      if( point.y() != mGrids.size() - 1 ) return Point( point.x() , point.y() + 1 );
      break;

    case down:

      if( point.y() != 0 ) return Point( point.x() , point.y() - 1 );
      break;

    case left:

      if( point.x() != 0 ) return Point( point.x() - 1 , point.y() );
      break;

    case right:

      if( point.x() != mGrids[0].size() - 1 ) return Point( point.x() + 1 , point.y() );
      break;

    default: break;
  }
  return nullPoint;
}

int MazeRouter::getFanin( const Point &point )
{
  int fanin = 0;

  for( int direct = 0 ; direct < directNum ; ++direct )
  {
     Point p = move( point , static_cast<Direct>( direct ) );

     if( p != nullPoint && mGrids[p.y()][p.x()].label() != Grid::OBSTACLE ) ++fanin;
  }
  return fanin;
}

void MazeRouter::setGridCost( Grid &grid, MazeRouter::MoveDirect direction ,
                              double costDiff )
{
  if      ( direction == horizontal ) grid.setCostY( grid.costY() + costDiff );
  else if ( direction == vertical   ) grid.setCostX( grid.costX() + costDiff );
}

