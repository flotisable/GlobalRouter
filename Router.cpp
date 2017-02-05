#include "Router.h"

#include <fstream>
#include <ostream>
#include <algorithm>
#include <map>
#include <regex>

// Router non-member functions
vector<const RoutingRegion*> getRegions( const RoutingGraph &graph )
{
  vector<const RoutingRegion*> regions;

  for( const Group &group : graph.groups() ) regions.push_back( &group );
  regions.push_back( &graph );

  return regions;
}

vector<Point> movePins( vector<Point> pins , const RoutingRegion *region , const Net &net )
{
  auto compare = []( const Point &front , const Point &back ) { return ( front.x() < back.x() ); };

  std::map<Point,Pin,decltype( compare )> table( compare );

  for( const Pin &pin : net.pins() ) table[region->map( pin.x() , pin.y() )] = pin;

  for( Point &p : pins )
  {
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
     for( const Point &target : pins )
        cost[i] += manhattanDistance( pins[i] , target );

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
  for( const Path &path : net.paths() )
     if( path.belongRegion() == region ) return true;
  return false;
}

void saveNet( Net &net , const RoutingRegion *region , const vector<Path> &paths )
{
  for( const Path &path : paths )
  {
     if( path.path().empty() ) continue;
     if( path.layer() == 0 )
     {
       vector<Point>  pins  { region->connectedPin( net ) };
       int            index { 0 };

       for( const Point &p : movePins( pins , region , net ) )
       {
          vector<Point> &pathT{ const_cast<vector<Point>&>( path.path() ) };
          auto          it    { find( pathT.begin() , pathT.end() , p )   };

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
          pathT.shrink_to_fit();
       }
     }
     net.paths().push_back( std::move( path ) );
  }

  for( Path &path : net.paths() )
     if( !path.belongRegion() ) path.setBelongRegion( region );
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

    for( const RoutingRegion *region : getRegions( graph ) )
    {
       std::cout << region->name() << std::endl;

       for( int layer = 0 ; layer <= maxLayer ; ++layer )
       {
          initRouter( region , layer );

          for( Net &net : graph.nets() )
          {
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
  std::ofstream file( fileName );

  file << graph;
}
// end Router public member functions

// Router private member functions
void Router::readGroup( const string &fileName )
{
  using namespace std;

  ifstream          file( fileName );
  string            buffer;
  vector<Symmetry>  symmetrys;

  const regex   symmetryPattern { R"(Symmetry (S[[:d:]]+) [[:d:]]+[[:s:]]*)" };
  const regex   groupPattern    { R"(Group (G[[:d:]]+) ([[:d:]]+) [NP][[:s:]]*)" };
  smatch        match;

  if( !file.is_open() ) throw FileOpenError{ fileName };

  while( !file.eof() )
  {
    getline( file , buffer );

    if      ( regex_match( buffer , match , symmetryPattern ) ) // read symmetry
    {
      Symmetry symmetry;

      symmetry.setName( match[1] );
      file >> buffer; symmetry.blocks().push_back( Block{ buffer } );
      file >> buffer; symmetry.blocks().push_back( Block{ buffer } );

      symmetrys.push_back( symmetry );
    }
    else if ( regex_match( buffer , match , groupPattern ) ) // read group
    {
      Group     group;
      const int blockNum = stoi( match[2] );

      group.setName( match[1] );

      for( int i = 0 ; i < blockNum ; ++i )
      {
         file >> buffer;

         if( regex_match( buffer , match , regex{ R"(S([[:d:]]+))" } ) )
           group.symmetrys().push_back( symmetrys[stoi( match[1] )-1] );
         else
           group.blocks().push_back( Block{ buffer } );
      }
      graph.groups().push_back( group );
    }
  }
}

void Router::readBlock( const string &fileName )
{
  using namespace std;

  const regex pattern{ R"(([[:d:]]+) ([[:d:]]+) ([[:d:]]+) ([[:d:]]+) ([[:alnum:]]+).*)" };

  ifstream  file( fileName );
  string    buffer;
  smatch    match;
  int       groupIndex = 0;

  if( !file.is_open() ) throw FileOpenError{ fileName };

  while( !file.eof() )
  {
    getline( file , buffer );

    if( regex_match( buffer , match , pattern ) )
    {
      Block  block( match[5] );
      
      /*
        因為 Block 設計的關係，要先設定 LeftBottom 再設定 Height , Width 才不會出問題
        because of the design of Block, we need to setup LeftBottom first, and then
        setup Height and Width
      */
      block.setLeftBottom ( stod( match[1] ) * unit , stod( match[2] ) * unit );
      block.setWidth      ( stod( match[3] ) * unit );
      block.setHeight     ( stod( match[4] ) * unit );

      if( block.name() == "ALL" )
      {
        graph = block;
      }
      else if( block.name()[0] == 'G' ) // 設定 Group set group
      {
        graph.groups()[groupIndex].setLeftBottom( block.leftBottom() );
        graph.groups()[groupIndex].setRightTop  ( block.rightTop  () );

        ++groupIndex;
      }
      else // 偵測 Block 是否屬於 Group  find if block is contained in a group
      {
        Block *blockPtr = graph.getBlock( block.name() );

        if( blockPtr )  *blockPtr = block;
        else            graph.blocks().push_back( block );
      }
    }
  }
  graph.buildSplit();
}

void Router::readNets( const string &fileName )
{
  using namespace std;

  const regex header{ R"(NetDegree[[:s:]]+:[[:s:]]+([[:d:]]+)[[:s:]]+([[:alnum:]!]+)[[:s:]]+([[:d:]]+).*)" };
  const regex data  { R"([[:s:]]*([[:alnum:]]+)[[:s:]]+O :[[:s:]]+([-[:d:]]+)[[:s:]]+([-[:d:]]+).*)" };

  ifstream  file( fileName );
  string    buffer;
  smatch    match;

  if( !file.is_open() ) throw FileOpenError{ fileName };

  while( !file.eof() )
  {
    getline( file , buffer );
    
    if( regex_match( buffer , match , header ) )
    {
      Net       net( match[2] , stoi( match[3] ) );
      const int pinNum = stoi( match[1] );

      for( int i = 0 ; i < pinNum ; ++i )
      {
         Pin    pin;
         Block  *block;

         getline( file , buffer );
         regex_match( buffer , match , data );

         pin.set( stod( match[2] ) * unit , stod( match[3] ) * unit );

         if( ( block = graph.getBlock( match[1] ) ) )
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