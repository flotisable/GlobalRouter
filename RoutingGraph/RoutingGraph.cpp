#include "RoutingGraph.h"

#include <algorithm>
#include <cassert>
#include <string>
using namespace std;

ostream& operator<<( ostream &out , const RoutingGraph &graph )
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
  for( const Group &group : graph.groups() ) out << group << endl;

  out << "Blocks : " << graph.blocks().size() << endl;
  for( const Block &block : graph.blocks() ) out << block << endl;
  out << endl;

  out << "Nets : " << graph.nets().size() << endl;
  for( const Net &net : graph.nets() ) out << net << endl;
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


vector<vector<Grid>> RoutingGraph::gridMap( int layer ) const
{
  vector<vector<Grid>> grids = RoutingRegion::gridMap( layer );

  for( const Group &group : groups() )
  {
     int xMin = getIndex( mHsplit , group.left  () );
     int xMax = getIndex( mHsplit , group.right () ) - 1;
     int yMin = getIndex( mVsplit , group.bottom() );
     int yMax = getIndex( mVsplit , group.top   () ) - 1;
     
     for( int i = yMin ; i <= yMax ; ++i )
        for( int j = xMin ; j <= xMax ; ++j )
        {
           grids[i][j].setLabel ( Grid::OBSTACLE );
           grids[i][j].setRegion( &group );
        }
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

vector<Point> RoutingGraph::connectedPin( const Net &net ) const
{
  vector<Point>         pins;
  vector<const Group*>  groups;

  for( const Pin &pin : net.pins() )
  {
     if(  ( mHsplit.front() <= pin.x() && pin.x() <= mHsplit.back() ) &&
          ( mVsplit.front() <= pin.y() && pin.y() <= mVsplit.back() ) )
     {
       unsigned int x;
       unsigned int y;

       for( x = 0 ; x < mHsplit.size() ; ++x )
          if( mHsplit[x] >= pin.x() )
          {
            --x;
            break;
          }
       for( y = 0 ; y < mVsplit.size() ; ++y )
          if( mVsplit[y] >= pin.y() )
          {
            --y;
            break;
          }
       
       for( const Group &belongGroup : this->groups() )
          if( belongGroup.getBlock( pin.connect()->name() ) )
          {
            for( const Group *group : groups )
               if( group == &belongGroup ) goto nextPin;
            groups.push_back( &belongGroup );
            pins.push_back( Point( x , y ) );
            goto nextPin;
          }
       pins.push_back( Point( x , y ) );
     }
     nextPin: continue;
  }
  return pins;
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

const Block* RoutingGraph::getBlock( const string &name ) const
{
  for( const Group &group : groups() )
  {
    const Block *block = group.getBlock( name );

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

const RoutingRegion* RoutingGraph::getRegion( const string &name ) const
{
  if( this->name() == name ) return this;

  for( const Group &group : groups() )
     if( group.name() == name ) return &group;

  return nullptr;
}

Net* RoutingGraph::getNet( const string &name )
{
  for( Net &net : nets() )
     if( net.name() == name ) return &net;

  return nullptr;
}

const Net* RoutingGraph::getNet( const string &name ) const
{
  for( const Net &net : nets() )
     if( net.name() == name ) return &net;

  return nullptr;
}

const Block& RoutingGraph::operator=( const Block &block )
{
  setName       ( block.name      () );
  setHeight     ( block.height    () );
  setWidth      ( block.width     () );
  setLeftBottom ( block.leftBottom().x() , block.leftBottom().y() );

  return block;
}
