#include "RoutingGraph.h"

#include <algorithm>
#include <cassert>
#include <string>
#include <cstdlib>
using namespace std;

ostream& operator<<( ostream &out , const RoutingGraph &graph )
{
  out << "[ Graph ]\n";
  out << "Horizontal Split : " << graph.hsplit().size() << endl;

  for( unsigned int i = 0 ; i < graph.hsplit().size() ; ++i )
     out << graph.hsplit()[i] << endl;
  out << endl;

  out << "Vertical Split : " << graph.vsplit().size() << endl;

  for( unsigned int i = 0 ; i < graph.vsplit().size() ; ++i )
     out << graph.vsplit()[i] << endl;
  out << endl;

  out << "Grids :\n";

  GridMap map = graph.gridMap();
  for( int i = map.row() - 1 ; i >=0 ; --i )
  {
     for( int j = 0 ; j < map.col() ; ++j )
        switch( map.grid( i , j ).label() )
        {
          case Grid::space:     out << "0"; break;
          case Grid::obstacle:  out << "1"; break;
          default:                          break;
        }
     out << endl;
  }
  out << endl;

  out << "Groups : " << graph.groups().size() << endl;
  for( unsigned int i = 0 ; i < graph.groups().size() ; ++i )
     out << graph.groups()[i] << endl;

  out << "Blocks : " << graph.blocks().size() << endl;
  for( unsigned int i = 0 ; i < graph.blocks().size() ; ++i )
     out << graph.blocks()[i] << endl;
  out << endl;

  out << "Nets : " << graph.nets().size() << endl;
  for( unsigned int i = 0 ; i < graph.nets().size() ; ++i )
     out << graph.nets()[i] << endl;
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
      int splitNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

      for( int i = 0 ; i < splitNum ; ++i )
      {
        getline( in , word );
        graph.hsplit().push_back( atof( word.data() ) );
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
      int splitNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

      for( int i = 0 ; i < splitNum ; ++i )
      {
        getline( in , word );
        graph.vsplit().push_back( atof( word.data() ) );
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
      int groupNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

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
      int blockNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

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
      int netNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

      for( int i = 0 ; i < netNum ; ++i )
      {
        Net net;

        in >> net;

        for( unsigned int i = 0 ; i < net.pins().size() ; ++i )
        {
           Block *block = net.pins()[i].connect();

           net.pins()[i].setConnect( graph.getBlock( block->name() ) );
           delete block;
        }
        for( unsigned int i = 0 ; i < net.paths().size() ; ++i )
        {
           RoutingRegion *region = net.paths()[i].belongRegion();

           net.paths()[i].setBelongRegion( graph.getRegion( region->name() ) );
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

  for( unsigned int i = 0 ; i < groups().size() ; ++i )
  {
     int xMin = getIndex( mHsplit , groups()[i].left  () );
     int xMax = getIndex( mHsplit , groups()[i].right () ) - 1;
     int yMin = getIndex( mVsplit , groups()[i].bottom() );
     int yMax = getIndex( mVsplit , groups()[i].top   () ) - 1;
     
     for( int j = yMin ; j <= yMax ; ++j )
        for( int k = xMin ; k <= xMax ; ++k )
        {
           map.grid( j , k ).setLabel ( Grid::obstacle );
           map.grid( j , k ).setBlock ( &groups()[i] );
        }
  }
  return map;
}

void RoutingGraph::buildSplit()
{
  mHsplit.push_back( left   () );
  mHsplit.push_back( right  () );
  mVsplit.push_back( top    () );
  mVsplit.push_back( bottom () );

  for( unsigned int i = 0 ; i < groups().size() ; ++i )
  {
     groups()[i].buildSplit();
     mHsplit.push_back( groups()[i].left   () );
     mHsplit.push_back( groups()[i].right  () );
     mVsplit.push_back( groups()[i].top    () );
     mVsplit.push_back( groups()[i].bottom () );
  }

  for( unsigned int i = 0 ; i < blocks().size() ; ++i )
  {
     mHsplit.push_back( blocks()[i].left   () );
     mHsplit.push_back( blocks()[i].right  () );
     mVsplit.push_back( blocks()[i].top    () );
     mVsplit.push_back( blocks()[i].bottom () );
  }

  sort( mHsplit.begin() , mHsplit.end() );
  sort( mVsplit.begin() , mVsplit.end() );

  vector<double>::iterator it = unique( mHsplit.begin() , mHsplit.end() );

  mHsplit.resize( distance( mHsplit.begin() , it ) );

  it = unique( mVsplit.begin() , mVsplit.end() );

  mVsplit.resize( distance( mVsplit.begin() , it ) );
}

vector<Point> RoutingGraph::connectedPin( const Net &net ) const
{
  vector<Point>         pins;
  vector<const Group*>  groups;

  for( unsigned int i = 0 ; i < net.pins().size() ; ++i )
  {
     const Pin &pin = net.pins()[i];

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
       
       for( unsigned int i = 0 ; i < this->groups().size() ; ++i )
       {
          const Group &belongGroup = this->groups()[i];

          if( belongGroup.getBlock( pin.connect()->name() ) )
          {
            for( unsigned int i = 0 ; i < groups.size() ; ++i )
               if( groups[i] == &belongGroup ) goto nextPin;
            groups.push_back( &belongGroup );
            pins.push_back( Point( x , y ) );
            goto nextPin;
          }
       }
       pins.push_back( Point( x , y ) );
     }
     nextPin: continue;
  }
  return pins;
}

Block* RoutingGraph::getBlock( const string &name )
{
  for( unsigned int i = 0 ; i < groups().size() ; ++i )
  {
    Block *block = groups()[i].getBlock( name );
    
    if( block ) return block;
  }
  return RoutingRegion::getBlock( name );
}

const Block* RoutingGraph::getBlock( const string &name ) const
{
  for( unsigned int i = 0 ; i < groups().size() ; ++i )
  {
    const Block *block = groups()[i].getBlock( name );

    if( block ) return block;
  }
  return RoutingRegion::getBlock( name );
}

RoutingRegion* RoutingGraph::getRegion( const string &name )
{
  if( this->name() == name ) return this;
  
  for( unsigned int i = 0 ; i < groups().size() ; ++i )
     if( groups()[i].name() == name ) return &groups()[i];

  return NULL;
}

const RoutingRegion* RoutingGraph::getRegion( const string &name ) const
{
  if( this->name() == name ) return this;

  for( unsigned int i = 0 ; i < groups().size() ; ++i )
     if( groups()[i].name() == name ) return &groups()[i];

  return NULL;
}

Net* RoutingGraph::getNet( const string &name )
{
  for( unsigned int i = 0 ; i < nets().size() ; ++i )
     if( nets()[i].name() == name ) return &nets()[i];

  return NULL;
}

const Net* RoutingGraph::getNet( const string &name ) const
{
  for( unsigned int i = 0 ; i < nets().size() ; ++i )
     if( nets()[i].name() == name ) return &nets()[i];

  return NULL;
}

const Block& RoutingGraph::operator=( const Block &block )
{
  setName       ( block.name      () );
  setHeight     ( block.height    () );
  setWidth      ( block.width     () );
  setLeftBottom ( block.leftBottom().x() , block.leftBottom().y() );

  return block;
}
