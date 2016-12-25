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
          insertPath( path );
       }
     }
     else return false;

     ++tag;
     source = target;
  }
  uniquePaths();
  cout << endl;
  return true;
}


Point MazeRouter::movePin( const Point &source, const Point &target )
{
  Point p = source;

  //while( !getFanin( p ) );
  p = target;
  return p;
}

bool MazeRouter::findPath( const Point &source , const Point &target )
{
  int           fanin       = getFanin( target );
  int           arrivedNum  = 0;
  int           label       = 1;
  queue<Point>  box;

  initSource( source );
  box.push( source );
  block = mGrids[target.y()][target.x()].block();

  while( !box.empty() )
  {
    const unsigned int boxNum = box.size();

    for( unsigned int i = 0 ; i < boxNum ; ++i , box.pop() )
    {
      for( int direct = 0 ; direct < directNum ; ++direct )
      {
         Point  p     = move( box.front() , static_cast<Direct>( direct ) );
         double cost  = mGrids[box.front().y()][box.front().x()].cost();

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
              !( grid.block() && grid.block() == block ) ) continue;

         for( int layer = 0 ; layer <= maxLayer ; ++layer )
         {
            double costDiff = getCostDiff( p , layer , static_cast<Direct>( direct ) );

            if( costDiff != nullCostDiff )
            {
              cost += costDiff;
              grid.setLayer( layer );
              goto pass;
            }
         }
         continue;
         pass:

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
  return ( arrivedNum > 0 );
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

       double costDiff = getCostDiff( pT , grid.layer() , static_cast<Direct>( direct ) );

       if( costDiff == nullCostDiff ) continue;

       if( grid.tag() == tag && grid.label() < label && grid.cost() <= cost )
       {
         pNext  = pT;
         cost   = grid.cost();
       }
    }
    // end move

    // set direction
    direction = getDirect( p , pNext );
    if( direction == unknown )
    {
      path[0].path().push_back( nullPoint );
      return path;
    }

    const Point &pPrevious = path[layer].path().back();

    if( pPrevious.x() != pNext.x() && pPrevious.y() != pNext.y() )
      path[layer].path().push_back( p );
    // end set direction

    // set cost
    Grid &grid = mGrids[pNext.y()][pNext.x()];

    setGridCost( mGrids[p.y()][p.x()] , layer , direction , wireWidthMin );

    if( layer != grid.layer() )
    {
      path[layer].path().push_back( pNext );
      layer = grid.layer();

      for( int i = static_cast<int>( path.size() ) ; i <= layer ; ++i )
         path.push_back( Path( i ) );
      path[layer].path().push_back( pNext );
    }
    p = pNext;
    
    switch( direction )
    {
      case up:    setGridCost( grid , layer , down  , wireWidthMin ); break;
      case down:  setGridCost( grid , layer , up    , wireWidthMin ); break;
      case left:  setGridCost( grid , layer , right , wireWidthMin ); break;
      case right: setGridCost( grid , layer , left  , wireWidthMin ); break;
      default: break;
    }
    // end set cost

    if( grid.label() != Grid::OBSTACLE )  label = grid.label();
    else                                  --label;
  }
  path[layer].path().push_back( source );

  cout << "trace success\n";
  return path;
}

void MazeRouter::insertPath( const Path &path )
{
  auto it = find_if(  mPaths.begin() , mPaths.end() ,
                      [&]( const Path &p ) { return ( p.layer() == path.layer() ); } );

  if( it == mPaths.end() ) mPaths.push_back( path );
  else                     it->path().insert( it->path().begin() ,
                                              path.path().begin() , path.path().end() );
}

void MazeRouter::uniquePaths()
{
  for( Path &pathT : mPaths )
  {
    vector<Point> &path = pathT.path();

    path.resize( distance( path.begin() , unique( path.begin() , path.end() ) ) );
  }
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
            !( grid.block() && grid.block() == block ) ) continue;
       ++fanin;
     }
  }
  return fanin;
}

double MazeRouter::getCostDiff( const Point &movedPoint , int layer ,
                                MazeRouter::Direct direction )
{
  const Grid  &grid     = mGrids[movedPoint.y()][movedPoint.x()];
  double      costDiff  = nullCostDiff;
  double      maxCost   = DBL_MAX;

  switch( direction )
  {
    case up:    costDiff = grid.costBottom( layer ); maxCost = gridMax.x(); break;
    case down:  costDiff = grid.costTop   ( layer ); maxCost = gridMax.x(); break;
    case left:  costDiff = grid.costRight ( layer ); maxCost = gridMax.y(); break;
    case right: costDiff = grid.costLeft  ( layer ); maxCost = gridMax.y(); break;
    default:                                                                break;
  }
  if( costDiff + wireWidthMin >= maxCost ) return nullCostDiff;
  return costDiff;
}

MazeRouter::Direct MazeRouter::getDirect( const Point &p, const Point &movedP )
{
  if      ( p.x() < movedP.x() )  return right;
  else if ( p.x() > movedP.x() )  return left;
  else if ( p.y() < movedP.y() )  return up;
  else if ( p.y() > movedP.y() )  return down;
  else                            return unknown;
}

void MazeRouter::setGridCost( Grid &grid , int layer , MazeRouter::Direct direction ,
                              double costDiff )
{
  switch( direction )
  {
    case up:    grid.setCostTop   ( grid.costTop    ( layer ) + costDiff , layer ); break;
    case down:  grid.setCostBottom( grid.costBottom ( layer ) + costDiff , layer ); break;
    case left:  grid.setCostLeft  ( grid.costLeft   ( layer ) + costDiff , layer ); break;
    case right: grid.setCostRight ( grid.costRight  ( layer ) + costDiff , layer ); break;
    default: break;
  }
}

void MazeRouter::output( const Point &source, const Point &target )
{
  auto precision  = cout.precision();
  int  tag        = mGrids[source.y()][source.x()].tag();

  cout << source << " -> " << target << endl;
  for( int i = static_cast<int>( mGrids.size() ) - 1 ; i >= 0 ; --i )
  {
     for( const Grid &grid : mGrids[i] ) cout << ( grid.tag() == tag );
     cout << " ";
     for( const Grid &grid : mGrids[i] )
        cout << setw( 2 ) << ( (  grid.label() == Grid::OBSTACLE ||
                                  grid.tag() == tag ) ? grid.label() : 0 );
     cout << " ";
     for( const Grid &grid : mGrids[i] )
        cout << setw( precision ) << ( ( grid.tag() == tag ) ? grid.cost() : 0.0 );
     cout << " ";
     for( const Grid &grid : mGrids[i] )
        cout << setw( 2 ) << ( ( grid.tag() == tag ) ? grid.layer() : 0 );
     cout << endl;
  }
  cout << endl;
}
