#include "Router.h"

#include <fstream>
#include <string>
#include <cctype>
#include <iostream>
using namespace std;

#include "Component/Block.h"

bool Router::readBlock( const string &fileName , const string &groupFileName )
{
  constexpr double unit = 0.01; // 0.01u

  ifstream  file( fileName );
  int       groupIndex = 0;

  if( !readGroup( groupFileName ) ) return false;
  if( !file.is_open() )
  {
    cerr << "cannot read " << fileName << endl;
    return false;
  }

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
        graph.groups()[groupIndex].setLeftBottom( lbX * unit , lbY * unit );
        graph.groups()[groupIndex].setHeight    ( height * unit );
        graph.groups()[groupIndex].setWidth     ( width * unit );
        
        groupIndex++;
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
  
  return true;
}

bool Router::readNets( const string &fileName )
{
  constexpr double unit = 0.01;

  ifstream  file( fileName );
  string    word;
  
  if( !file.is_open() )
  {
    cerr << "cannot read " << fileName << endl;
    return false;
  }
  
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
  return true;
}

bool Router::route()
{
  if( graph.vsplit().size() == 0 || graph.hsplit().size() == 0 ) return false;

  vector<RoutingRegion*> regions;

  for( Group &group : graph.groups() ) regions.push_back( &group );
  regions.push_back( &graph );

  for( RoutingRegion *region : regions )
  {
     cout << region->name() << endl;
     mRouter->setGrids( region->gridMap() );
     mRouter->setGridMax( region->maxGridWidth() , region->maxGridHeight() );

     for( Net &net : graph.nets() )
     {
        if( !region->netConnected( net ) ) continue;

        cout << net.name() << endl;
        mRouter->setPins( region->connectedPin( net ) );
        mRouter->route();
        mRouter->saveNet( net );

        for( Path &path : net.paths() )
           if( !path.belongRegion() )
             path.setBelongRegion( region );
     }
  }
  return true;
}

void Router::outputData( const string &fileName ) const
{
  ofstream file( fileName );
  
  file << graph;
}


bool Router::readGroup( const string &fileName )
{
  ifstream          file( fileName );
  string            word;
  vector<Symmetry>  symmetrys;

  if( !file.is_open() )
  {
    cerr << "cannot read " << fileName << endl;
    return false;
  }

  while( !file.eof() )
  {
    file >> word;
    
    if      ( word == "Symmetry" )
    {
      Symmetry symmetry;

      file >> word;
      
      if( word[0] != 'S' )  continue;
      else                  symmetry.setName( word );
      file >> word; // pass one word

      file >> word; symmetry.blocks().push_back( Block( word ) );
      file >> word; symmetry.blocks().push_back( Block( word ) );
      
      symmetrys.push_back( symmetry );
    }
    else if ( word == "Group" )
    {
      Group group;
      int   blockNum;
      
      file >> word;
      
      if( word[0] != 'G' )  continue;
      else                  group.setName( word );
      
      file >> blockNum >> word; // pass one word
      
      for( int i = 0 ; i < blockNum ; ++i )
      {
         file >> word;
         
         if( word[0] == 'S' )
         {
           int index = stoi( word.substr( 1 ) ) - 1;
           
           group.symmetrys().push_back( symmetrys[index] );
         }
         else
           group.blocks().push_back( Block( word ) );
      }
      graph.groups().push_back( group );
    }
  }
  return true;
}
