#include "Router.h"

#include <fstream>
#include <string>
#include <cctype>
#include <iostream>
using namespace std;

#include "Component/Block.h"

bool Router::readBlock( const string &fileName , const string &groupFileName )
{
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
    // �P�_���� test if it is comment
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
    // end �P�_���� test if it is comment
    
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
        �]�� Block �]�p�����Y�A�n���]�w LeftBottom �A�]�w Height , Width �~���|�X���D
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
      else if( name[0] == 'G' ) // �]�w Group set group
      {
        graph.groups()[groupIndex].setLeftBottom( lbX * unit , lbY * unit );
        graph.groups()[groupIndex].setHeight    ( height * unit );
        graph.groups()[groupIndex].setWidth     ( width * unit );
        
        ++groupIndex;
      }
      else // ���� Block �O�_�ݩ� Group  find if block is contained in a group
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

  for( RoutingRegion *region : getRegions() )
  {
     int layer;

     cout << region->name() << endl;

     for( layer = 0 ; layer <= maxLayer ; ++layer )
     {
        initRouter( region , layer );

        for( Net &net : graph.nets() )
        {
           if( netRouted( net , region ) || !region->netConnected( net ) ) continue;

           cout << net.name() << endl;
           mRouter->setPins( region->connectedPin( net ) );
           if( !mRouter->route() ) goto nextLayer;
           saveNet( net , region );
        }
        break;
        nextLayer: continue;
     }
     if( layer > maxLayer ) return false;
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
      symmetry.setName( word );
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
      group.setName( word );
      
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
  mRouter->setGrids   ( region->gridMap( 1 + maxLayer ) );
  mRouter->setGridMax ( region->maxGridWidth() , region->maxGridHeight() );
}

bool Router::netRouted( const Net &net , const RoutingRegion *region )
{
  for( const Path &path : net.paths() )
     if( path.belongRegion() == region ) return true;
  return false;
}

void Router::saveNet( Net &net , RoutingRegion *region )
{
  mRouter->saveNet( net );

  for( Path &path : net.paths() )
     if( !path.belongRegion() ) path.setBelongRegion( region );
}
