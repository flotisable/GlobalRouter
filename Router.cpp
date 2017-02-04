#include "Router.h"

#include <fstream>
#include <string>
#include <cctype>
#include <iostream>
#include <cfloat>
#include <algorithm>
#include <map>
#include <regex>
using namespace std;

#include "Component/Block.h"

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

    for( RoutingRegion *region : getRegions() )
    {
       cout << region->name() << endl;

       for( int layer = 0 ; layer <= maxLayer ; ++layer )
       {
          initRouter( region , layer );

          for( Net &net : graph.nets() )
          {
             if( !region->netConnected( net ) ) continue;

             cout << net.name() << endl;
             mRouter->setPins( sortPins( movePins( region->connectedPin( net ) , region , net ) ) );
             if( !mRouter->route() ) goto nextLayer;
             if( netRouted( net , region ) ) continue;
             saveNet( net , region );
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
  ofstream file( fileName );
  
  file << graph;
}
// end Router public member functions

// Router private member functions
void Router::readGroup( const string &fileName )
{
  ifstream          file( fileName );
  string            buffer;
  vector<Symmetry>  symmetrys;

  const regex   symmetryPattern { R"(Symmetry (S[[:digit:]]+) [[:digit:]]+[[:blank:]]*)" };
  const regex   groupPattern    { R"(Group (G[[:digit:]]+) ([[:digit:]]+) [NP][[:blank:]]*)" };
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

         if( regex_match( buffer , match , regex{ R"(S([[:digit:]]+))" } ) )
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
  ifstream  file( fileName );
  int       groupIndex = 0;

  if( !file.is_open() ) throw FileOpenError( fileName );

  while( !file.eof() )
  {
    // 判斷註解 test if it is comment
    if( file.peek() == '/' )
    {
      char c = file.get();

      if( file.peek() == '/' )
      {
        while( file.get() != '\n' );
        continue;
      }
      else file.putback( c );
    }
    // end 判斷註解 test if it is comment

    if( isdigit( file.peek() ) )
    {
      Block  block;
      double lbX;
      double lbY;
      double height;
      double width;
      string name;
    
      file >> lbX >> lbY >> width >> height >> name;
      
      /*
        因為 Block 設計的關係，要先設定 LeftBottom 再設定 Height , Width 才不會出問題
        because of the design of Block, we need to setup LeftBottom first, and then
        setup Height and Width
      */
      block.setName       ( name );
      block.setLeftBottom ( lbX * unit , lbY *unit );
      block.setHeight     ( height * unit );
      block.setWidth      ( width * unit );

      if( name == "ALL" )
      {
        graph = block;
      }
      else if( name[0] == 'G' ) // 設定 Group set group
      {
        graph.groups()[groupIndex].setLeftBottom( block.leftBottom() );
        graph.groups()[groupIndex].setRightTop  ( block.rightTop  () );
        
        ++groupIndex;
      }
      else // 偵測 Block 是否屬於 Group  find if block is contained in a group
      {
        for( Group &group : graph.groups() )
        {
           Block* blockPtr = group.getBlock( block.name() );

           if( blockPtr )
           {
             *blockPtr = block;
             goto match;
           }
        }
        graph.blocks().push_back( block );
      }
    }
    match:

    while( file.get() != '\n' )
      if( file.eof() ) break;
  }
  graph.buildSplit();
}

void Router::readNets( const string &fileName )
{
  ifstream  file( fileName );
  string    word;
  
  if( !file.is_open() ) throw FileOpenError( fileName );
  
  while( !file.eof() )
  {
    file >> word;
    
    if( word == "NetDegree" )
    {
      Net net;
      int currentDensity;
      int pinNum;
      
      file >> word >> pinNum >> word >> currentDensity;
      net.setName           ( word );
      net.setCurrentDensity ( currentDensity );
      
      for( int i = 0 ; i < pinNum ; i++ )
      {
         Pin    pin;
         string blockName;
         Block* block;
         double x;
         double y;

         file >> blockName >> word >> word >> x >> y;
         
         pin.set( x * unit , y * unit );

         if( ( block = graph.getBlock( blockName ) ) )
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

vector<RoutingRegion*> Router::getRegions()
{
  vector<RoutingRegion*> regions;

  for( Group &group : graph.groups() ) regions.push_back( &group );
  regions.push_back( &graph );

  return regions;
}

void Router::initRouter( const RoutingRegion *region, int maxLayer )
{
  mRouter->setMaxLayer( maxLayer );
  mRouter->setGridMap ( region->gridMap( 1 + maxLayer ) );
  mRouter->setGridMax ( region->maxGridWidth() , region->maxGridHeight() );
}

vector<Point> Router::movePins( vector<Point> pins , const RoutingRegion *region , const Net &net )
{
  auto compare = []( const Point &front , const Point &back ) { return ( front.x() < back.x() ); };

  map<Point,Pin,decltype( compare )> table( compare );

  for( const Pin &pin : net.pins() ) table[region->map( pin.x() , pin.y() )] = pin;

  for( Point &p : pins )
  {
     const Point lb = region->map( table[p].connect()->leftBottom() );
     Point rt = region->map( table[p].connect()->rightTop  () );

     rt = Point{ rt.x() - 1 , rt.y() - 1 };

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

vector<Point> Router::sortPins( vector<Point> pins )
{
  if( pins.size() <= 2 ) return pins;

  vector<double> cost( pins.size() , 0 );

  // find first pin to be route
  for( unsigned int i = 0 ; i < pins.size() ; ++i )
  {
     const Point &pin = pins[i];

     for( const Point &target : pins )
     {
        if( pin == target ) continue;
        cost[i] += manhattanDistance( pin , target );
     }
  }

  int firstIndex = 0;

  for( unsigned int i = 1 ; i < cost.size() ; ++i )
     if( cost[i] < cost[firstIndex] ) firstIndex = i;

  swap( pins[0] , pins[firstIndex] );
  // end find first pin to be route

  for( unsigned int i = 1 ; i < pins.size() - 1 ; ++i )
  {
     const Point  &source   = pins[i-1];
     int          nextIndex = i;

     for( unsigned int j = i + 1 ; j < pins.size() ; ++j )
        if( manhattanDistance( source , pins[nextIndex] ) > manhattanDistance( source , pins[j] ) ) nextIndex = j;

     swap( pins[i] , pins[nextIndex] );
  }
  return pins;
}

bool Router::netRouted( const Net &net , const RoutingRegion *region )
{
  for( const Path &path : net.paths() )
     if( path.belongRegion() == region ) return true;
  return false;
}

void Router::saveNet( Net &net , RoutingRegion *region )
{
  for( const Path &path : mRouter->paths() )
     if( !path.path().empty() )
     {
       if( path.layer() == 0 )
       {
         for( const Point &p : movePins( region->connectedPin( net ) , region , net ) )
         {
//            const Point   p       = region->map( pin.x() , pin.y() );
            vector<Point> &pathT  = const_cast<vector<Point>&>( path.path() );

            auto it = find( pathT.begin() , pathT.end() , p );

            if      ( it      == pathT.end  () ) continue;
            else if ( it + 1  == pathT.end  () ) ++it;
            else if ( it      != pathT.begin() )
            {
              pathT.insert( it , *it );
              it = find( pathT.begin() , pathT.end() , p );
              ++it;
            }

            pathT.insert( it , p );
         }
       }
       net.paths().push_back( std::move( path ) );
     }

  for( Path &path : net.paths() )
     if( !path.belongRegion() ) path.setBelongRegion( region );
}
// end Router private member functions