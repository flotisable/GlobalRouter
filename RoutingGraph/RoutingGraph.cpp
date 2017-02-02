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
  GridMap map = graph.gridMap();
  for( int i = map.row() - 1 ; i >=0 ; --i )
  {
     for( const Grid &grid : map.grid( i ) )
        switch( grid.value() )
        {
          case Grid::Value::space:    out << "0"; break;
          case Grid::Value::obstacle: out << "1"; break;
          default:                                break;
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


GridMap RoutingGraph::gridMap( int layer ) const
{
  GridMap map = RoutingRegion::gridMap( layer );

  for( const Group &group : groups() )
  {
     int xMin = getIndex( hsplit() , group.left  () );
     int xMax = getIndex( hsplit() , group.right () ) - 1;
     int yMin = getIndex( vsplit() , group.bottom() );
     int yMax = getIndex( vsplit() , group.top   () ) - 1;
     
     for( int i = yMin ; i <= yMax ; ++i )
        for( int j = xMin ; j <= xMax ; ++j )
        {
           map.grid( i , j ).setValue( Grid::Value::obstacle );
           map.grid( i , j ).setBlock( &group );
        }
  }
  return map;
}

void RoutingGraph::buildSplit()
{
  for( Group &group : groups() )
  {
     group.buildSplit();
     hsplit().push_back( group.left   () );
     hsplit().push_back( group.right  () );
     vsplit().push_back( group.top    () );
     vsplit().push_back( group.bottom () );
  }
  RoutingRegion::buildSplit();
}

vector<Point> RoutingGraph::connectedPin( const Net &net ) const
{
  vector<Point>         pins;
  vector<const Group*>  groups;

  for( const Pin &pin : net.pins() )
  {
     if(  ( hsplit().front() <= pin.x() && pin.x() <= hsplit().back() ) &&
          ( vsplit().front() <= pin.y() && pin.y() <= vsplit().back() ) )
     {
       unsigned int x;
       unsigned int y;

       for( x = 0 ; x < hsplit().size() ; ++x )
          if( hsplit()[x] >= pin.x() )
          {
            --x;
            break;
          }
       for( y = 0 ; y < vsplit().size() ; ++y )
          if( vsplit()[y] >= pin.y() )
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
  setLeftBottom ( block.leftBottom() );
  setRightTop   ( block.rightTop  () );

  return block;
}
