#include "RoutingGraph.h"

#include <algorithm>
#include <cassert>
#include <string>
using namespace std;

ostream& operator<<( ostream &out , RoutingGraph &graph )
{
  out << "[ Graph ]\n";
  out << "Horizontal Split : " << graph.hsplit().size() << endl;

  for( double point : graph.hsplit() ) out << point << endl;
  out << endl;

  out << "Vertical Split : " << graph.vsplit().size() << endl;

  for( double point : graph.vsplit() ) out << point << endl;
  out << endl;

  out << "Grids :\n";
  vector<vector<Grid>> grids = graph.gridMap();
  for( int i = static_cast<int>( grids.size() ) - 1 ; i >=0 ; --i )
  {
     for( const Grid &grid : grids[i] )
        switch( grid.label() )
        {
          case Grid::SPACE:     out << "0"; break;
          case Grid::OBSTACLE:  out << "1"; break;
          default:                          break;
        }
     out << endl;
  }
  out << endl;

  out << "Groups : " << graph.groups().size() << endl;
  for( Group &group : graph.groups() ) out << group << endl;

  out << "Blocks : " << graph.blocks().size() << endl;
  for( const Block &block : graph.blocks() ) out << block << endl;
  out << endl;

  out << "Nets : " << graph.nets().size() << endl;
  for( Net &net : graph.nets() ) out << net << endl;
  out << endl;
  
  return out;
}

istream& operator>>( istream &in  , RoutingGraph &graph )
{
  string word;

  graph.setName( "ALL" );

  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Horizontal Split : " ) != string::npos )
    {
      int splitNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

      for( int i = 0 ; i < splitNum ; ++i )
      {
        getline( in , word );
        graph.hsplit().push_back( stod( word ) );
      }
      graph.setLeft  ( graph.hsplit().front () );
      graph.setRight ( graph.hsplit().back  () );
      break;
    }
  }

  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Vertical Split : " ) != string::npos )
    {
      int splitNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

      for( int i = 0 ; i < splitNum ; ++i )
      {
        getline( in , word );
        graph.vsplit().push_back( stod( word ) );
      }
      graph.setBottom( graph.vsplit().front () );
      graph.setTop   ( graph.vsplit().back  () );
      break;
    }
  }

  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Groups : " ) != string::npos )
    {
      int groupNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

      for( int i = 0 ; !in.eof() && i < groupNum ; )
      {
        getline( in , word );

        if( word.find( "[ Group : " ) != string::npos )
        {
          Group         group;
          unsigned int  start = word.find( ": " ) + 2;
          unsigned int  end   = word.rfind( ' ' );

          group.setName( word.substr( start , end - start ) );

          in >> group;

          graph.groups().push_back( group );
          ++i;
        }
      }
      break;
    }
  }

  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Blocks : " ) != string::npos )
    {
      int blockNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

      for( int i = 0 ; i < blockNum ; ++i )
      {
        Block block;

        in >> block;
        graph.blocks().push_back( block );
      }
      break;
    }
  }

  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Nets : " ) != string::npos )
    {
      int netNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

      for( int i = 0 ; i < netNum ; ++i )
      {
        Net net;

        in >> net;

        for( Pin &pin : net.pins() )
        {
           Block *block = pin.connect();

           pin.setConnect( graph.getBlock( block->name() ) );
           delete block;
        }
        for( Path &path : net.paths() )
        {
           RoutingRegion *region = path.belongRegion();

           path.setBelongRegion( graph.getRegion( region->name() ) );
           delete region;
        }
        graph.nets().push_back( net );

        getline( in , word );
      }
    }
  }
  return in;
}


vector<vector<Grid>> RoutingGraph::gridMap()
{
  assert( mVsplit.size() > 0 );
  assert( mHsplit.size() > 0 );
  vector<vector<Grid>> grids( mVsplit.size() - 1 , vector<Grid>( mHsplit.size() - 1 ) );

  for( const Group &group : groups() )
  {
     int x = getIndex( mHsplit , group.left  () );
     int y = getIndex( mVsplit , group.bottom() );

     grids[y][x].setLabel( Grid::OBSTACLE );
  }

  for( const Block &block : mBlocks )
  {
     int x = getIndex( mHsplit , block.left  () );
     int y = getIndex( mVsplit , block.bottom() );

     grids[y][x].setLabel( Grid::OBSTACLE );
  }

  double maxH = 0;
  double maxV = 0;

  for( unsigned int i = 0 ; i < mHsplit.size() - 1 ; ++i )
     if( mHsplit[i+1] - mHsplit[i] > maxH ) maxH = mHsplit[i+1] - mHsplit[i];

  for( unsigned int i = 0 ; i < mVsplit.size() - 1 ; ++i )
     if( mVsplit[i+1] - mVsplit[i] > maxV ) maxV = mVsplit[i+1] - mVsplit[i];

  for( unsigned int i = 0 ; i < grids.size() ; ++i )
     for( unsigned int j = 0 ; j < grids[0].size() ; ++j )
     {
        grids[i][j].setCostX( maxH - ( mHsplit[j+1] - mHsplit[j] ) );
        grids[i][j].setCostY( maxV - ( mVsplit[i+1] - mVsplit[i] ) );
     }

  return grids;
}

void RoutingGraph::buildSplit()
{
  mHsplit.push_back( left   () );
  mHsplit.push_back( right  () );
  mVsplit.push_back( top    () );
  mVsplit.push_back( bottom () );

  for( Group &group : groups() )
  {
     group.buildSplit();
     mHsplit.push_back( group.left   () );
     mHsplit.push_back( group.right  () );
     mVsplit.push_back( group.top    () );
     mVsplit.push_back( group.bottom () );
  }

  for( const Block &block : blocks() )
  {
     mHsplit.push_back( block.left   () );
     mHsplit.push_back( block.right  () );
     mVsplit.push_back( block.top    () );
     mVsplit.push_back( block.bottom () );
  }

  sort( mHsplit.begin() , mHsplit.end() );
  sort( mVsplit.begin() , mVsplit.end() );

  auto it = unique( mHsplit.begin() , mHsplit.end() );

  mHsplit.resize( distance( mHsplit.begin() , it ) );
  mHsplit.shrink_to_fit();

  it = unique( mVsplit.begin() , mVsplit.end() );

  mVsplit.resize( distance( mVsplit.begin() , it ) );
  mVsplit.shrink_to_fit();
}

Block* RoutingGraph::getBlock( const string &name )
{
  for( Group &group : groups() )
  {
    Block *block = group.getBlock( name );

    if( block ) return block;
  }
  return RoutingRegion::getBlock( name );
}

RoutingRegion* RoutingGraph::getRegion( const string &name )
{
  if( this->name() == name ) return this;

  for( Group &group : groups() )
     if( group.name() == name ) return &group;

  return nullptr;
}

Block RoutingGraph::operator=( const Block &block )
{
  setName       ( block.name      () );
  setHeight     ( block.height    () );
  setWidth      ( block.width     () );
  setLeftBottom ( block.leftBottom().x() , block.leftBottom().y() );

  return block;
}
