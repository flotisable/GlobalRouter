#include "MazeRouter.h"

#include <queue>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <limits>

const Point MazeRouter::nullPoint = Point( -1 , -1 );

// MazeRouter public member functions
bool MazeRouter::route()
{
  Point source = pins.front();

  mPaths.clear();

  for( const Point &target : pins )
  {
     std::cout << target << std::endl;

     if( source == target ) continue;

     sourceBlock = getBlock( source );
     targetBlock = getBlock( target );

     if( findPath( source , target ) )
     {
       for( const Path &path : backTrace( source , target ) )
          if( !path.path().empty() ) insertPath( path );
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
  for( Path &pathT : paths )
  {
    vector<Point> &path = pathT.path();

    path.resize( distance( path.begin() , unique( path.begin() , path.end() ) ) );
  }
}

MazeRouter::Direct getDirect( const Point &p, const Point &movedP )
{
  if      ( p.x() < movedP.x() )  return MazeRouter::Direct::right;
  else if ( p.x() > movedP.x() )  return MazeRouter::Direct::left;
  else if ( p.y() < movedP.y() )  return MazeRouter::Direct::up;
  else if ( p.y() > movedP.y() )  return MazeRouter::Direct::down;
  else                            return MazeRouter::Direct::unknown;
}

void output( const Point &source, const Point &target , const GridMap &map )
{
  using namespace std;

  auto precision  = cout.precision();
  int  tag        = map.grid( source.y() , source.x() ).tag();

  cout << source << " -> " << target << endl;
  for( int i = map.row() - 1 ; i >= 0 ; --i )
  {
     // output tag
     for( const Grid &grid : map.grid( i ) ) cout << ( grid.tag() == tag );
     cout << " ";
     // end output tag
     // output label
     for( const Grid &grid : map.grid( i ) )
        cout << setw( 2 ) << ( ( grid.tag() == tag ) ? grid.label() : 0 );
     cout << " ";
     // end output label
     // output cost
     for( const Grid &grid : map.grid( i ) )
        cout << setw( precision ) << ( ( grid.tag() == tag ) ? grid.cost() : 0.0 );
     // end output cost
     cout << endl;
  }
  cout << endl;
}

MazeRouter::Direct& operator++( MazeRouter::Direct &direct )
{
  switch( direct )
  {
    case MazeRouter::Direct::up:
    case MazeRouter::Direct::down:
    case MazeRouter::Direct::left:
    case MazeRouter::Direct::right:

      return direct = static_cast<MazeRouter::Direct>( static_cast<int>( direct ) + 1 );

    default: return direct = MazeRouter::Direct::unknown;
  }
}
// end MazeRouter non-member functions

// MazeRouter private member functions
const Block *MazeRouter::getBlock( const Point &point )
{
  const Block *block = map.grid( point.y() , point.x() ).block();

  return ( block && block->type() == Block::Type::region ) ? block : nullptr;
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

       for( Direct direct = Direct::init ; direct != Direct::final ; ++direct )
       {
          if( p == target )
          {
            ++arrivedNum;
            continue;
          }

          Point  pMoved = move( p , direct );
          double cost   = map.grid( p.y() , p.x() ).cost();

          if( ( pMoved == nullPoint || gridBlocked( pMoved ) ) && pMoved != target ) continue;

          // select layer
          int layer = 0;

          for( ; layer <= maxLayer ; ++layer )
          {
             double costDiff = getCostDiff( p , layer , direct );

             if( costDiff == nullCostDiff ) continue;

             cost += costDiff;
             break;
          }
          if( layer > maxLayer ) continue;
          // end select layer

          if( setGridInfo( pMoved , label , cost , direct , layer ) )
            box.push( pMoved );
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
  double        cost      = std::numeric_limits<double>::max();
  int           label     = map.grid( target.y() , target.x() ).label();
  int           layer     = 0;
  Direct        direction = Direct::unknown;
  vector<Path>  path( 1 );

  path[0].path().push_back( target );

  while( p != source )
  {
    Point pNext;

    // move
    for( Direct direct = Direct::init ; direct != Direct::final ; ++direct )
    {
       Point pT = move( p , direct );

       if( pT == nullPoint || move( pT , direction ) == p ) continue;

       Grid &grid = map.grid( pT.y() , pT.x() );
       int  layer = map.grid( p.y() , p.x() )
                    .edge( static_cast<Grid::Direct>( direct ) )->layer();

       double costDiff = getCostDiff( p , layer , direct );

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
    if( direction == Direct::unknown ) throw BacktraceError();

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
  auto it = find_if(  mPaths.begin() , mPaths.end() ,
                      [&]( const Path &p ) { return ( p.layer() == path.layer() ); } );

  if( it == mPaths.end() )  mPaths.push_back( path );
  else                      it->path().insert(  it->path().end() ,
                                                path.path().begin() , path.path().end() );
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
    case Direct::up:    return ( point.y() != map.row() - 1 ) ? Point{ point.x() , point.y() + 1 } : nullPoint;
    case Direct::down:  return ( point.y() != 0             ) ? Point{ point.x() , point.y() - 1 } : nullPoint;
    case Direct::left:  return ( point.x() != 0             ) ? Point{ point.x() - 1 , point.y() } : nullPoint;
    case Direct::right: return ( point.x() != map.col() - 1 ) ? Point{ point.x() + 1 , point.y() } : nullPoint;
    default:            return nullPoint;
  }
}

int MazeRouter::getFanin( const Point &point )
{
  int fanin{};

  for( Direct direct = Direct::init ; direct != Direct::final ; ++direct )
  {
     Point p = move( point , direct );

     if( p != nullPoint && !gridBlocked( p ) ) ++fanin;
  }
  return fanin;
}

bool MazeRouter::gridBlocked( const Point &point )
{
  Grid &grid = map.grid( point.y() , point.x() );

  return (  grid.value() == Grid::Value::obstacle &&
            !( grid.block() && ( grid.block() == sourceBlock || grid.block() == targetBlock ) ) );
}

double MazeRouter::getCostDiff( const Point &point , int layer ,
                                MazeRouter::Direct direction )
{
  const Grid  &grid   = map.grid( point.y() , point.x() );
  double      maxCost;

  if      ( direction == Direct::up   || direction == Direct::down  ) maxCost = gridMax.x();
  else if ( direction == Direct::left || direction == Direct::right ) maxCost = gridMax.y();
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
    case Direct::up:    direct = Grid::bottom;  break;
    case Direct::down:  direct = Grid::top;     break;
    case Direct::left:  direct = Grid::right;   break;
    case Direct::right: direct = Grid::left;    break;
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