#include "Net.h"

std::ostream& operator<<( std::ostream &out , const Net &net )
{
  using std::endl;

  out << net.name() << " " << net.currentDensity() << " "
      << net.pins().size() << " "<< net.paths().size() << endl;
  
  for( const Pin &pin : net.pins() ) out << pin << " ";
  out << endl;
  
  for( const Path &path : net.paths() ) out << path << endl;

  return out;
}

std::istream& operator>>( std::istream &in  , Net &net )
{
  string  word;
  string  name;
  double  currentDensity;
  int     pinNum;
  int     pathNum;

  in >> name >> currentDensity >> pinNum >> pathNum;

  net.setName( name );
  net.setCurrentDensity( currentDensity );

  for( int i = 0 ; i < pinNum ; ++i )
  {
    Pin pin;
  
    in >> pin;

    net.pins().push_back( pin );
  }
  std::getline( in , word );

  for( int i = 0 ; i < pathNum ; ++i )
  {
    Path path;

    in >> path;

    net.paths().push_back( path );
    std::getline( in , word );
  }
  return in;
}
