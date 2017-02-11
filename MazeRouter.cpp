#include "MazeRouter.h"

#include <queue>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <limits>

const Point   MazeRouter::nullPoint     = Point( -1 , -1 );
const double  MazeRouter::nullCostDiff  = -1.0;

// MazeRouter public member functions
bool MazeRouter::route()
{
  Point source = pins.front();

  mPaths.clear();

  for( unsigned int i = 0 ; i < pins.size() ; ++i )
  {
     const Point &target = pins[i];

     std::cout << target << std::endl;

     if( source == target ) continue;

     sourceBlock = getBlock( source );
     targetBlock = getBlock( target );

     if( findPath( source , target ) )
     {
       vector<Path> paths = backTrace( source , target );

       for( unsigned int i = 0 ; i < paths.size() ; ++i )
          if( !paths[i].path().empty() ) insertPath( paths[i] );
     }
     else return false;

     ++tag;
     source = target;
  }
  uniquePaths( mPaths );
  std::cout << std::endl;
  return true;
}
// end MazeRouter public member functions

// MazeRouter non-member functions
void uniquePaths( vector<Path> &paths )
{
  for( unsigned int i = 0 ; i < paths.size() ; ++i )
  {
    vector<Point> &path = paths[i].path();

    path.resize( distance( path.begin() , unique( path.begin() , path.end() ) ) );
  }
}

MazeRouter::Direct getDirect( const Point &p, const Point &movedP )
{
  if      ( p.x() < movedP.x() )  return MazeRouter::right;
  else if ( p.x() > movedP.x() )  return MazeRouter::left;
  else if ( p.y() < movedP.y() )  return MazeRouter::up;
  else if ( p.y() > movedP.y() )  return MazeRouter::down;
  else                            return MazeRouter::unknown;
}

void output( const Point &source, const Point &target , const GridMap &map )
{
  using namespace std;

  streamsize  precision = cout.precision();
  int         tag       = map.grid( source.y() , source.x() ).tag();

  cout << source << " -> " << target << endl;
  for( int i = map.row() - 1 ; i >= 0 ; --i )
  {
     vector<Grid> row = map.grid( i );

     // output tag
     for( unsigned int i = 0 ; i < row.size() ; ++i ) cout << ( row[i].tag() == tag );
     cout << " ";
     // end output tag
     // output label
     for( unsigned int i = 0 ; i < row.size() ; ++i )
        cout << setw( 2 ) << ( ( row[i].tag() == tag ) ? row[i].label() : 0 );
     cout << " ";
     // end output label
     // output cost
     for( unsigned int i = 0 ; i < row.size() ; ++i )
        cout << setw( precision ) << ( ( row[i].tag() == tag ) ? row[i].cost() : 0.0 );
     // end output cost
     cout << endl;
  }
  cout << endl;
}

MazeRouter::Direct& operator++( MazeRouter::Direct &direct )
{
  switch( direct )
  {
    case MazeRouter::up:
    case MazeRouter::down:
    case MazeRouter::left:
    case MazeRouter::right:

      return direct = static_cast<MazeRouter::Direct>( static_cast<int>( direct ) + 1 );

    default: return direct = MazeRouter::unknown;
  }
}
// end MazeRouter non-member functions

// MazeRouter private member functions
const Block *MazeRouter::getBlock( const Point &point )
{
  const Block *block = map.grid( point.y() , point.x() ).block();

  return ( block && block->type() == Block::region ) ? block : NULL;
}

bool MazeRouter::findPath( const Point &source , const Point &target )
{
  int               fanin       = getFanin( target );
  int               arrivedNum  = 0;
  int               label       = 1;
  std::queue<Point> box;

  if( fanin == 0 )
  {
    targetBlock = map.grid( target.y() , target.x() ).block();
    fanin       = getFanin( target );
  }

  initSource( source );
  box.push( source );

  while( !box.empty() )
  {
    const unsigned int boxNum = box.size();

    for( unsigned int i = 0 ; i < boxNum ; ++i , box.pop() )
    {
       const Point &p = box.front();

       if( p == target )
       {
         ++arrivedNum;
         continue;
       }

       for( int direct = 0 ; direct < directNum ; ++direct )
       {
          Point     pMoved = move( p , static_cast<Direct>( direct ) );
          CostType  cost   = map.grid( p.y() , p.x() ).cost();

          if( pMoved == nullPoint || gridBlocked( pMoved ) && pMoved != target ) continue;

          // select layer
          int layer = 0;

          for( ; layer <= maxLayer ; ++layer )
          {
             CostType costDiff = getCostDiff( p , layer , static_cast<Direct>( direct ) );

             if( costDiff == nullCostDiff ) continue;

             cost += costDiff;
             break;
          }
          if( layer > maxLayer ) continue;
          // end select layer

          if( setGridInfo( pMoved , label , cost , static_cast<Direct>( direct ) , layer ) ) box.push( pMoved );
      }
    }
    ++label;
  }
  if( arrivedNum > 0 )
  {
    output( source , target , map );
    return true;
  }
  return false;
}

vector<Path> MazeRouter::backTrace( const Point &source , const Point &target )
{
  Point         p         = target;
  CostType      cost      = std::numeric_limits<double>::max();
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

       Grid       &grid = map.grid( pT.y() , pT.x() );
       const int  layer = map.grid( p.y() , p.x() ).edge( static_cast<Grid::Direct>( direct ) )->layer();

       if( getCostDiff( p , layer , static_cast<Direct>( direct ) ) == nullCostDiff ) continue;

       if( grid.tag() == tag && grid.label() < label && grid.cost() <= cost )
       {
         pNext  = pT;
         cost   = grid.cost();
       }
    }
    // end move

    // set direction
    if( ( direction = getDirect( p , pNext ) ) == unknown ) throw BacktraceError();

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

  std::cout << "trace success\n";
  return path;
}

void MazeRouter::insertPath( const Path &path )
{
  unsigned int i = 0;

  for( ; i < mPaths.size() ; ++i )
     if( mPaths[i].layer() == path.layer() ) break;

  if( i == mPaths.size() ) mPaths.push_back( path );
  else
  {
    mPaths[i].path().insert(  mPaths[i].path().begin() ,
                              path.path().begin() , path.path().end() );
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
    case up:    return ( point.y() != map.row() - 1 ) ? Point( point.x() , point.y() + 1 ) : nullPoint;
    case down:  return ( point.y() != 0             ) ? Point( point.x() , point.y() - 1 ) : nullPoint;
    case left:  return ( point.x() != 0             ) ? Point( point.x() - 1 , point.y() ) : nullPoint;
    case right: return ( point.x() != map.col() - 1 ) ? Point( point.x() + 1 , point.y() ) : nullPoint;
    default:    return nullPoint;
  }
}

int MazeRouter::getFanin( const Point &point )
{
  int fanin = 0;

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

  return (  grid.value() == Grid::obstacle &&
            !( grid.block() && ( grid.block() == sourceBlock || grid.block() == targetBlock ) ) );
}

double MazeRouter::getCostDiff( const Point &point , int layer ,
                                MazeRouter::Direct direction )
{
  const Grid  &grid   = map.grid( point.y() , point.x() );
  double      maxCost;

  if      ( direction == up   || direction == down  ) maxCost = gridMax.x();
  else if ( direction == left || direction == right ) maxCost = gridMax.y();
  else                                                                return nullCostDiff;

  double costDiff = grid.edge( static_cast<Grid::Direct>( direction ) )->cost( layer );

  if( costDiff + wireWidthMin >= maxCost ) return nullCostDiff;
  return costDiff + abs( grid.layer() - layer ) * std::max( gridMax.x() , gridMax.y() );
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
    default:            return false;
  }

  if( grid.tag() != tag || cost < grid.cost() )
  {
    grid.setTag  ( tag );
    grid.setLabel( label );
    grid.setLayer( layer );
    grid.setCost ( cost );
    grid.edge( direct )->setLayer( layer );
    return true;
  }
  return false;
}
// end MazeRouter private member functions
