#include "Router.h"

#include <fstream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <sstream>

// Router non-member functions
vector<const RoutingRegion*> getRegions( const RoutingGraph &graph )
{
  vector<const RoutingRegion*> regions;

  for( unsigned int i = 0 ; i < graph.groups().size() ; ++i ) regions.push_back( &graph.groups()[i] );
  regions.push_back( &graph );

  return regions;
}

vector<Point> movePins( vector<Point> pins , const RoutingRegion *region , const Net &net )
{
  std::map<Point,Pin> table;

  for( unsigned int i = 0 ; i < net.pins().size() ; ++i )
     table[region->map( net.pins()[i].x() , net.pins()[i].y() )] = net.pins()[i];

  for( unsigned int i = 0 ; i < pins.size() ; ++i )
  {
     Point        &p = pins[i];
     const Point  lb = region->map( table[p].connect()->leftBottom() );
     Point        rt = region->map( table[p].connect()->rightTop  () );

     rt.set( rt.x() - 1 , rt.y() - 1 );

     if(  lb.x() < p.x() && p.x() < rt.x() &&
          lb.y() < p.y() && p.y() < rt.y() )
     {
       double y = ( p.y() - lb.y() > rt.y() - p.y() ) ? rt.y() : lb.y();

       if       ( y == region->bottom () ) y = rt.y();
       else if  ( y == region->top    () ) y = lb.y();

       p.setY( y );
     }
  }
  return pins;
}

vector<Point> sortPins( vector<Point> pins )
{
  if( pins.size() <= 2 ) return pins;

  vector<double> cost( pins.size() , 0 );

  // find first pin to be route
  for( unsigned int i = 0 ; i < pins.size() ; ++i )
     for( unsigned int j = 0 ; j < pins.size() ; ++j )
        cost[i] += manhattanDistance( pins[i] , pins[j] );

  int firstIndex = 0;

  for( unsigned int i = 1 ; i < cost.size() ; ++i )
     if( cost[i] < cost[firstIndex] ) firstIndex = i;

  std::swap( pins[0] , pins[firstIndex] );
  // end find first pin to be route

  for( unsigned int i = 1 ; i < pins.size() - 1 ; ++i )
  {
     const Point  &source   = pins[i-1];
     int          nextIndex = i;

     for( unsigned int j = i + 1 ; j < pins.size() ; ++j )
        if( manhattanDistance( source , pins[nextIndex] ) > manhattanDistance( source , pins[j] ) ) nextIndex = j;

     std::swap( pins[i] , pins[nextIndex] );
  }
  return pins;
}

bool netRouted( const Net &net , const RoutingRegion *region )
{
  for( unsigned int i = 0 ; i < net.paths().size() ; ++i )
     if( net.paths()[i].belongRegion() == region ) return true;
  return false;
}

void saveNet( Net &net , const RoutingRegion *region , const vector<Path> &paths )
{
  for( unsigned int i = 0 ; i < paths.size() ; ++i )
  {
     const Path &path = paths[i];

     if( path.path().empty() ) continue;
     if( path.layer() == 0 )
     {
       vector<Point>  pins      = region->connectedPin( net );
       vector<Point>  pinsMoved = movePins( pins , region , net );
       int            index     = 0 ;

       for( unsigned int i = 0 ; i < pinsMoved.size() ; ++i )
       {
          const Point             &p      = pinsMoved[i];
          vector<Point>           &pathT  = const_cast<vector<Point>&>( path.path() );
          vector<Point>::iterator it      = find( pathT.begin() , pathT.end() , p );

          if      ( it      == pathT.end  () ) continue;
          else if ( it + 1  == pathT.end  () ) ++it;
          else if ( it      != pathT.begin() )
          {
            pathT.insert( it , *it );
            it = find( pathT.begin() , pathT.end() , p );
            ++it;
          }
          pathT.insert( it , pins[index++] );

          it = unique( pathT.begin() , pathT.end() );
          pathT.resize( distance( pathT.begin() , it ) );
       }
     }
     net.paths().push_back( path );
  }

  for( unsigned int i = 0 ; i < net.paths().size() ; ++i )
  {
     Path &path = net.paths()[i];

     if( !path.belongRegion() ) path.setBelongRegion( region );
  }
}
// end Router non-member functions

// Router public member functions
void Router::read( const string &groupFileName , const string &blockFileName , const string &netFileName )
{
  readGroup ( groupFileName );
  readBlock ( blockFileName );
  readNets  ( netFileName   );
}

void Router::route()
{
  try
  {
    if( graph.vsplit().size() == 0 || graph.hsplit().size() == 0 ) return;

    vector<const RoutingRegion*> regions = getRegions( graph );

    for( unsigned int i = 0 ; i < regions.size() ; ++i )
    {
       const RoutingRegion *region = regions[i];

       std::cout << region->name() << std::endl;

       for( int layer = 0 ; layer <= maxLayer ; ++layer )
       {
          initRouter( region , layer );

          for( unsigned int i = 0 ; i < graph.nets().size() ; ++i )
          {
             Net &net = graph.nets()[i];

             if( !region->netConnected( net ) ) continue;

             std::cout << net.name() << std::endl;
             mRouter->setPins( sortPins( movePins( region->connectedPin( net ) , region , net ) ) );
             if( !mRouter->route() ) goto nextLayer;
             if( netRouted( net , region ) ) continue;
             saveNet( net , region , mRouter->paths() );
          }
          break;
          nextLayer: if( layer >= maxLayer ) throw NetCannotRoute();
      }
    }
  }
  catch( const MazeRouter::BacktraceError &error )
  {
    throw RoutingEngineError( "MazeRouter" , "BacktraceError" );
  }
}

void Router::outputData( const string &fileName ) const
{
  std::ofstream file( fileName.data() );

  file << graph;
}
// end Router public member functions

// Router private member functions
void Router::readGroup( const string &fileName )
{
  using namespace std;

  ifstream          file( fileName.data() );
  string            buffer;
  vector<Symmetry>  symmetrys;

  if( !file.is_open() ) throw FileOpenError( fileName );

  while( !file.eof() )
  {
    getline( file , buffer );

    istringstream line( buffer );
    string        buffer;

    line >> buffer;

    if      ( buffer == "Symmetry" ) // read symmetry
    {
      Symmetry symmetry;

      line >> buffer;

      if( buffer[0] != 'S' ) continue;

      symmetry.setName( buffer );
      file >> buffer; symmetry.blocks().push_back( Block( buffer ) );
      file >> buffer; symmetry.blocks().push_back( Block( buffer ) );

      symmetrys.push_back( symmetry );
    }
    else if ( buffer == "Group" ) // read group
    {
      Group group;
      int   blockNum;

      line >> buffer >> blockNum;

      if( buffer[0] != 'G' ) continue;

      group.setName( buffer );

      for( int i = 0 ; i < blockNum ; ++i )
      {
         file >> buffer;

         if( buffer[0] == 'S' )
           group.symmetrys().push_back( symmetrys[atoi( buffer.substr( 1 ).data() )-1] );
         else
           group.blocks().push_back( Block( buffer ) );
      }
      graph.groups().push_back( group );
    }
  }
}

void Router::readBlock( const string &fileName )
{
  using namespace std;

  ifstream  file( fileName.data() );
  string    buffer;
  int       groupIndex = 0;

  if( !file.is_open() ) throw FileOpenError( fileName );

  while( !file.eof() )
  {
    getline( file , buffer );

    istringstream line( buffer );
    double        x;
    double        y;
    double        width;
    double        height;
    Block         block;

    line >> x >> y >> width >> height >> buffer;

    if( !line ) continue;
    /*
      ? ç‚º Block è¨­è??„é?ä¿‚ï?è¦å?è¨­å? LeftBottom ?è¨­å®?Height , Width ?ä??ƒå‡º?é?
      because of the design of Block, we need to setup LeftBottom first, and then
      setup Height and Width
    */
    block.setName       ( buffer );
    block.setLeftBottom ( x * unit , y * unit );
    block.setWidth      ( width   * unit );
    block.setHeight     ( height  * unit );

    if( block.name() == "ALL" )
    {
      graph = block;
    }
    else if( block.name()[0] == 'G' ) // è¨­å? Group set group
    {
      graph.groups()[groupIndex].setLeftBottom( block.leftBottom() );
      graph.groups()[groupIndex].setRightTop  ( block.rightTop  () );

      ++groupIndex;
    }
    else // ?µæ¸¬ Block ?¯å¦å±¬æ–¼ Group  find if block is contained in a group
    {
      Block *blockPtr = graph.getBlock( block.name() );

      if( blockPtr )  *blockPtr = block;
      else            graph.blocks().push_back( block );
    }
  }
  graph.buildSplit();
}

void Router::readNets( const string &fileName )
{
  using namespace std;

  ifstream  file( fileName.data() );
  string    buffer;

  if( !file.is_open() ) throw FileOpenError( fileName );

  while( !file.eof() )
  {
    getline( file , buffer );

    istringstream line( buffer );

    line >> buffer;

    if( buffer == "NetDegree" )
    {
      Net net;
      int pinNum;
      int currentDensity;

      line >> buffer >> pinNum >> buffer >> currentDensity;

      net.setName           ( buffer );
      net.setCurrentDensity ( currentDensity );

      for( int i = 0 ; i < pinNum ; ++i )
      {
         Pin    pin;
         Block  *block;
         string name;
         double x;
         double y;

         line.clear();
         getline( file , buffer );
         line.str( buffer );

         line >> name >> buffer >> buffer >> x >> y;

         pin.set( x * unit , y * unit );

         if( ( block = graph.getBlock( name ) ) )
         {
           pin.setConnect( block );
           pin += block->center();
         }
         net.pins().push_back( pin );
      }
      graph.nets().push_back( net );
    }
  }
}

void Router::initRouter( const RoutingRegion *region , int maxLayer )
{
  mRouter->setMaxLayer( maxLayer );
  mRouter->setGridMap ( region->gridMap( 1 + maxLayer ) );
  mRouter->setGridMax ( region->maxGridWidth() , region->maxGridHeight() );
}
// end Router private member functions