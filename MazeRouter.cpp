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
  Point source = pins.front();

  paths.clear();

  for( const Point &target : pins )
  {
     cout << target << endl;

     if( source == target ) continue;

     block = map.grid( target.y() , target.x() ).block();

     if( findPath( source , target ) )
     {
       for( const Path &path : backTrace( source , target ) )
       {
          if( path.path().empty() ) continue;
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

  while( !box.empty() )
  {
    const unsigned int boxNum = box.size();

    for( unsigned int i = 0 ; i < boxNum ; ++i , box.pop() )
    {
       const Point &p = box.front();

       for( int direct = 0 ; direct < directNum ; ++direct )
       {
          if( p == target )
          {
            if( ++arrivedNum == fanin )
            {
              output( source , target );
              return true;
            }
            continue;
          }

          Point  pMoved = move( p , static_cast<Direct>( direct ) );
          double cost   = map.grid( p.y() , p.x() ).cost();

          if( pMoved == nullPoint || gridBlocked( pMoved ) ) continue;

          // select layer
          int layer = 0;

          for( ; layer <= maxLayer ; ++layer )
          {
             double costDiff = getCostDiff( p , layer , static_cast<Direct>( direct ) );

             if( costDiff == nullCostDiff ) continue;

             cost += costDiff;
             break;
          }
          if( layer > maxLayer ) continue;
          // end select layer

          if( setGridInfo( pMoved , label , cost , static_cast<Direct>( direct ) , layer ) )
            box.push( pMoved );
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
  int           label     = map.grid( target.y() , target.x() ).label();
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

       if( pT == nullPoint || move( pT , direction ) == p ) continue;

       Grid &grid = map.grid( pT.y() , pT.x() );
       int  layer = map.grid( p.y() , p.x() )
                    .edge( static_cast<Grid::Direct>( direct ) )->layer();

       double costDiff = getCostDiff( p , layer , static_cast<Direct>( direct ) );

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

    Edge *edge = map.grid( p.y() , p.x() ).edge( static_cast<Grid::Direct>( direction ) );

    edge->setCost( edge->cost( edge->layer() ) + wireWidthMin , edge->layer() );

    if( layer != edge->layer() )
    {
      path[layer].path().push_back( p );
      layer = edge->layer();

      for( int i = static_cast<int>( path.size() ) ; i <= layer ; ++i )
         path.push_back( Path( i ) );
      path[layer].path().push_back( p );
    }
    p     = pNext;
    label = map.grid( p.y() , p.x() ).label();
  }
  path[layer].path().push_back( source );

  cout << "trace success\n";
  return path;
}

void MazeRouter::insertPath( const Path &path )
{
  auto it = find_if(  paths.begin() , paths.end() ,
                      [&]( const Path &p ) { return ( p.layer() == path.layer() ); } );

  if( it == paths.end() ) paths.push_back( path );
  else                    it->path().insert(  it->path().begin() ,
                                              path.path().begin() , path.path().end() );
}

void MazeRouter::uniquePaths()
{
  for( Path &pathT : paths )
  {
    vector<Point> &path = pathT.path();

    path.resize( distance( path.begin() , unique( path.begin() , path.end() ) ) );
  }
}

void MazeRouter::initSource( const Point &source )
{
  Grid &grid = map.grid( source.y() , source.x() );

  grid.setTag   ( tag );
  grid.setLabel ( 0 );
  grid.setCost  ( 0 );
}

Point MazeRouter::move( const Point &point, MazeRouter::Direct direction )
{
  switch( direction )
  {
    case up:

      if( point.y() != map.row() - 1 ) return Point( point.x() , point.y() + 1 );
      break;

    case down:

      if( point.y() != 0 ) return Point( point.x() , point.y() - 1 );
      break;

    case left:

      if( point.x() != 0 ) return Point( point.x() - 1 , point.y() );
      break;

    case right:

      if( point.x() != map.col() - 1 ) return Point( point.x() + 1 , point.y() );
      break;

    default: break;
  }
  return nullPoint;
}

int MazeRouter::getFanin( const Point &point )
{
  int fanin{};

  for( int direct = 0 ; direct < directNum ; ++direct )
  {
     Point p = move( point , static_cast<Direct>( direct ) );

     if( p != nullPoint && !gridBlocked( p ) ) ++fanin;
  }
  return fanin;
}

bool MazeRouter::gridBlocked( const Point &point )
{
  Grid &grid = map.grid( point.y() , point.x() );

  return ( grid.value() == Grid::Value::obstacle && !( grid.block() && grid.block() == block ) );
}

double MazeRouter::getCostDiff( const Point &point , int layer ,
                                MazeRouter::Direct direction )
{
  const Grid  &grid   = map.grid( point.y() , point.x() );
  double      maxCost = DBL_MAX;

  if      ( direction == up   || direction == down  ) maxCost = gridMax.x();
  else if ( direction == left || direction == right ) maxCost = gridMax.y();
  else                                                return nullCostDiff;

  double costDiff = grid.edge( static_cast<Grid::Direct>( direction ) )->cost( layer );

  if( costDiff + wireWidthMin >= maxCost ) return nullCostDiff;
  return costDiff;
}

bool MazeRouter::setGridInfo( const Point &point, int label, double cost,
                              Direct direction , int layer )
{
  Grid          &grid = map.grid( point.y() , point.x() );
  Grid::Direct  direct;

  switch( direction )
  {
    case up:    direct = Grid::bottom;  break;
    case down:  direct = Grid::top;     break;
    case left:  direct = Grid::right;   break;
    case right: direct = Grid::left;    break;
    default:    return false;
  }

  if( grid.tag() != tag )
  {
    grid.setTag  ( tag );
    grid.setLabel( label );
    grid.setCost ( cost );
    grid.edge( direct )->setLayer( layer );
    return true;
  }
  else if( cost < grid.cost() )
  {
    grid.setCost( cost );
    grid.edge( direct )->setLayer( layer );
    return true;
  }
  return false;
}

MazeRouter::Direct MazeRouter::getDirect( const Point &p, const Point &movedP )
{
  if      ( p.x() < movedP.x() )  return right;
  else if ( p.x() > movedP.x() )  return left;
  else if ( p.y() < movedP.y() )  return up;
  else if ( p.y() > movedP.y() )  return down;
  else                            return unknown;
}

void MazeRouter::output( const Point &source, const Point &target )
{
  auto precision  = cout.precision();
  int  tag        = map.grid( source.y() , source.x() ).tag();

  cout << source << " -> " << target << endl;
  for( int i = map.row() - 1 ; i >= 0 ; --i )
  {
     for( const Grid &grid : map.grid( i ) ) cout << ( grid.tag() == tag );
     cout << " ";
     for( const Grid &grid : map.grid( i ) )
        cout << setw( 2 ) << ( ( grid.tag() == tag ) ? grid.label() : 0 );
     cout << " ";
     for( const Grid &grid : map.grid( i ) )
        cout << setw( precision ) << ( ( grid.tag() == tag ) ? grid.cost() : 0.0 );
     cout << " ";
     cout << endl;
  }
  cout << endl;
}
