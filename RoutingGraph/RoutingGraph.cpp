#include "RoutingGraph.h"

#include <string>

// RoutingGraph non-member functions
std::ostream& operator<<( std::ostream &out , const RoutingGraph &graph )
{
  using std::endl;

  out << "[ Graph ]\n";
  out << "Horizontal Split : " << graph.hsplit().size() << endl;

  for( double point : graph.hsplit() ) out << point << endl;
  out << endl;

  out << "Vertical Split : " << graph.vsplit().size() << endl;

  for( double point : graph.vsplit() ) out << point << endl;
  out << endl;

  out << "Grids :\n";
  outputGridMapValue( out , graph.gridMap() );

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
      const int splitNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

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
  // end read horizontal split

  // read vertical split
  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Vertical Split : " ) != string::npos )
    {
      const int splitNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

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
  // end read vertical split

  // read groups
  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Groups : " ) != string::npos )
    {
      const int groupNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

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
  // end read blocks

  // read nets
  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Nets : " ) != string::npos )
    {
      const int netNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

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
  // end read nets
  return in;
}
// end RoutingGraph non-member functions

// RoutingGraph member functions
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
       for( const Group &belongGroup : this->groups() )
          if( belongGroup.getBlock( pin.connect()->name() ) )
          {
            for( const Group *group : groups )
               if( group == &belongGroup ) goto nextPin;
            groups.push_back( &belongGroup );
            break;
          }
       pins.push_back( map( pin.x() , pin.y() ) );
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
  return const_cast<RoutingGraph*>( this )->getBlock( name );
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
  return const_cast<RoutingGraph*>( this )->getRegion( name );
}

Net* RoutingGraph::getNet( const string &name )
{
  for( Net &net : nets() )
     if( net.name() == name ) return &net;

  return nullptr;
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