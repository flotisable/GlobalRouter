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

  mPaths.clear();

  for( const Point &target : mPins )
  {
     cout << target << endl;

     if( source == target ) continue;

     if( findPath( source , target ) )
     {
       for( const Path &path : backTrace( source , target ) )
       {
          if( path.path().back() == nullPoint ) return false;

          auto it = find_if(  mPaths.begin() , mPaths.end() ,
                              [&]( const Path &p )
                              { return ( p.layer() == path.layer() ); } );

          if( it == mPaths.end() )
            mPaths.push_back( path );
          else
            it->path().insert(  it->path().begin() ,
                                path.path().begin() , path.path().end() );
       }
     }
     else return false;

     ++tag;
     source = target;
  }

  for( Path &pathT : mPaths )
  {
    vector<Point> &path = pathT.path();

    auto it = unique( path.begin() , path.end() );

    path.resize( distance( path.begin() , it ) );
  }
  cout << endl;
  return true;
}


bool MazeRouter::findPath( const Point &source , const Point &target )
{
  int           fanin       = getFanin( target );
  int           arrivedNum  = 0;
  int           label       = 1;
  queue<Point>  box;

  initSource( source );
  box.push( source );

  region = mGrids[target.y()][target.x()].region();
  
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
             output( source , target );
             return true;
           }
           continue;
         }

         Grid &grid = mGrids[p.y()][p.x()];

         if(  grid.label() == Grid::OBSTACLE &&
              !( grid.region() && grid.region() == region ) ) continue;

         switch( direct )
         {
           case up:
           
             if( grid.costBottom() + wireWidthMin >= gridMax.x() ) continue;
             cost += grid.costBottom();
             break;

           case down:
           
             if( grid.costTop() + wireWidthMin >= gridMax.x() ) continue;
             cost += grid.costTop();
             break;
           
           case left:
           
             if( grid.costRight() + wireWidthMin >= gridMax.y() ) continue;
             cost += grid.costRight();
             break;
           
           case right:

             if( grid.costLeft() +wireWidthMin >= gridMax.y() ) continue;
             cost += grid.costLeft();
             break;

         }

         if( grid.tag() != tag )
         {
           grid.setTag  ( tag );
           if( grid.label() != Grid::OBSTACLE ) grid.setLabel( label );
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

vector<Path> MazeRouter::backTrace( const Point &source , const Point &target )
{
  Point         p         = target;
  double        cost      = DBL_MAX;
  int           label     = INT_MAX;
  int           layer     = 0;
  Direct        direction = unknown;
  vector<Path>  path( 1 );

  path[0].path().push_back( target );
  
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

       switch( direct )
       {
         case up:     if( grid.costBottom () + wireWidthMin >= gridMax.x() ) continue;
                      break;
         case down:   if( grid.costTop    () + wireWidthMin >= gridMax.x() ) continue;
                      break;
         case left:   if( grid.costRight  () + wireWidthMin >= gridMax.y() ) continue;
                      break;
         case right:  if( grid.costLeft   () + wireWidthMin >= gridMax.y() ) continue;
                      break;
       }

       if( grid.label() < label && grid.cost() <= cost )
       {
         pNext  = pT;
         cost   = grid.cost();
       }
    }
    // end move

    // set direction
    if      ( p.x() < pNext.x() )
    {
      if( direction == up || direction == down ) path[0].path().push_back( p );
      direction = right;
    }
    else if ( p.x() > pNext.x() )
    {
      if( direction == up || direction == down ) path[0].path().push_back( p );
      direction = left;
    }
    else if ( p.y() < pNext.y() )
    {
      if( direction == left || direction == right ) path[0].path().push_back( p );
      direction = up;
    }
    else if ( p.y() > pNext.y() )
    {
      if( direction == left || direction == right ) path[0].path().push_back( p );
      direction = down;
    }
    else
    {
      path[0].path().push_back( nullPoint );
      return path;
    }
    // end set direction

    setGridCost( mGrids[p.y()][p.x()] , layer , direction , wireWidthMin );
    p = pNext;

    Grid &grid = mGrids[p.y()][p.x()];
    
    switch( direction )
    {
      case up:    setGridCost( grid , layer , down  , wireWidthMin ); break;
      case down:  setGridCost( grid , layer , up    , wireWidthMin ); break;
      case left:  setGridCost( grid , layer , right , wireWidthMin ); break;
      case right: setGridCost( grid , layer , left  , wireWidthMin ); break;
      default: break;
    }

    if( grid.label() != Grid::OBSTACLE )  label = grid.label();
    else                                  --label;
  }
  path[0].path().push_back( source );

  cout << "trace success\n";

  return path;
}

void MazeRouter::initSource( const Point &source )
{
  Grid &s = mGrids[source.y()][source.x()];

  s.setTag( tag );
  if( s.label() != Grid::OBSTACLE ) s.setLabel( 0 );
  s.setCost( 0 );
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

     if( p != nullPoint )
     {
       Grid &grid = mGrids[p.y()][p.x()];

       if(  grid.label() == Grid::OBSTACLE &&
            !( grid.region() && grid.region() == region ) ) continue;
       ++fanin;
     }
  }
  return fanin;
}

void MazeRouter::setGridCost( Grid &grid , int layer , MazeRouter::Direct direction ,
                              double costDiff )
{
  switch( direction )
  {
    case up:    grid.setCostTop   ( grid.costTop    () + costDiff , layer ); break;
    case down:  grid.setCostBottom( grid.costBottom () + costDiff , layer ); break;
    case left:  grid.setCostLeft  ( grid.costLeft   () + costDiff , layer ); break;
    case right: grid.setCostRight ( grid.costRight  () + costDiff , layer ); break;
    default: break;
  }
}

void MazeRouter::output( const Point &source, const Point &target )
{
  auto precision = cout.precision();

  cout << source << " -> " << target << endl;
  for( int i = static_cast<int>( mGrids.size() ) - 1 ; i >= 0 ; --i )
  {
     for( const Grid &grid : mGrids[i] ) cout << setw( 2 ) << grid.tag();
     cout << " ";
     for( const Grid &grid : mGrids[i] ) cout << setw( 2 ) << grid.label();
     cout << " ";
     for( const Grid &grid : mGrids[i] ) cout << setw( precision ) << grid.cost();
     cout << endl;
  }
  cout << endl;
}
