#include "RoutingGraph.h"

#include <string>
#include <cstdlib>

// RoutingGraph non-member functions
std::ostream& operator<<( std::ostream &out , const RoutingGraph &graph )
{
  using std::endl;

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
  outputGridMapValue( out , graph.gridMap() );

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

std::istream& operator>>( std::istream &in  , RoutingGraph &graph )
{
  string word;

  graph.setName( "ALL" );

  // read horizontal split
  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Horizontal Split : " ) != string::npos )
    {
      const int splitNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

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
  // end read horizontal split

  // read vertical split
  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Vertical Split : " ) != string::npos )
    {
      const int splitNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

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
  // end read vertical split

  // read groups
  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Groups : " ) != string::npos )
    {
      const int groupNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

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
  // end read groups

  // read blocks
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
  // end read blocks

  // read nets
  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Nets : " ) != string::npos )
    {
      const int netNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

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
           const RoutingRegion *region = net.paths()[i].belongRegion();

           net.paths()[i].setBelongRegion( graph.getRegion( region->name() ) );
           delete region;
        }
        graph.nets().push_back( net );

        getline( in , word );
      }
    }
  }
  // end read nets
  return in;
}
// end RoutingGraph non-member functions

// RoutingGraph member functions
GridMap RoutingGraph::gridMap( int layer ) const
{
  GridMap map = RoutingRegion::gridMap( layer );

  for( unsigned int i = 0 ; i < groups().size() ; ++i )
  {
     int xMin = getIndex( hsplit() , groups()[i].left  () );
     int xMax = getIndex( hsplit() , groups()[i].right () ) - 1;
     int yMin = getIndex( vsplit() , groups()[i].bottom() );
     int yMax = getIndex( vsplit() , groups()[i].top   () ) - 1;
     
     for( int j = yMin ; j <= yMax ; ++j )
        for( int k = xMin ; k <= xMax ; ++k )
        {
           map.grid( j , k ).setValue ( Grid::obstacle );
           map.grid( j , k ).setBlock ( &groups()[i] );
        }
  }
  return map;
}

void RoutingGraph::buildSplit()
{
  for( unsigned int i = 0 ; i < groups().size() ; ++i )
  {
     groups()[i].buildSplit();
     hsplit().push_back( groups()[i].left   () );
     hsplit().push_back( groups()[i].right  () );
     vsplit().push_back( groups()[i].top    () );
     vsplit().push_back( groups()[i].bottom () );
  }
  RoutingRegion::buildSplit();
}

vector<Point> RoutingGraph::connectedPin( const Net &net ) const
{
  vector<Point>         pins;
  vector<const Group*>  groups;

  for( unsigned int i = 0 ; i < net.pins().size() ; ++i )
  {
     const Pin &pin = net.pins()[i];

     if(  ( hsplit().front() <= pin.x() && pin.x() <= hsplit().back() ) &&
          ( vsplit().front() <= pin.y() && pin.y() <= vsplit().back() ) )
     {       
       for( unsigned int i = 0 ; i < this->groups().size() ; ++i )
       {
          const Group &belongGroup = this->groups()[i];

          if( belongGroup.getBlock( pin.connect()->name() ) )
          {
            for( unsigned int i = 0 ; i < groups.size() ; ++i )
               if( groups[i] == &belongGroup ) goto nextPin;
            groups.push_back( &belongGroup );
            break;
          }
       }
       pins.push_back( map( pin.x() , pin.y() ) );
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
  return const_cast<RoutingGraph*>( this )->getBlock( name );
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
  return const_cast<RoutingGraph*>( this )->getRegion( name );
}

Net* RoutingGraph::getNet( const string &name )
{
  for( unsigned int i = 0 ; i < nets().size() ; ++i )
     if( nets()[i].name() == name ) return &nets()[i];

  return NULL;
}

const Net* RoutingGraph::getNet( const string &name ) const
{
  return const_cast<RoutingGraph*>( this )->getNet( name );
}

const Block& RoutingGraph::operator=( const Block &block )
{
  setName       ( block.name      () );
  setLeftBottom ( block.leftBottom() );
  setRightTop   ( block.rightTop  () );

  return block;
}
// end RoutingGraph member functions