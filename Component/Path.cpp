#include "Path.h"

#include "../RoutingGraph/RoutingRegion.h"

// Path non-member functions
std::ostream& operator<<( std::ostream &out , const Path &path )
{
  out << path.belongRegion()->name() << " " << path.layer() << " "
      << path.path().size() << " ";
  
  for( const Point &p : path.path() ) out << p << " ";

  return out;
}

std::istream& operator>>( std::istream &in  , Path &path )
{
  RoutingRegion *region = new RoutingRegion{};
  string        name;
  int           pathLayer;
  int           pathNum;

  in >> name >> pathLayer >> pathNum;

  region->setName( name );

  path.setLayer( pathLayer );
  path.setBelongRegion( region );

  for( int i = 0 ; i < pathNum ; ++i )
  {
     Point point;

     in >> point;
     path.path().push_back( point );
  }
  return in;
}
// end Path non-member functions
