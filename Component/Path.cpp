#include "Path.h"

#include "../RoutingGraph/RoutingGraph.h"
#include "Pin.h"

std::ostream& operator<<( std::ostream &out , Path &path )
{
  using std::endl;
  
  out << path.belongRegion()->name() << " " << path.path().size() << " ";
  
  for( const Point &p : path.path() ) out << p << " ";

  return out;
}

std::istream& operator>>( std::istream &in  , Path &path )
{
  RoutingRegion *region = new RoutingGraph;
  string        name;
  int           pathNum;

  in >> name >> pathNum;

  region->setName( name );
  
  path.setBelongRegion( region );

  for( int i = 0 ; i < pathNum ; ++i )
  {
     Point point;
     
     in >> point;
     
     path.path().push_back( point );
  }
  return in;
}
