#include "Net.h"

std::ostream& operator<<( std::ostream &out , Net &net )
{
  using std::endl;

  out << net.name() << " " << net.currentDensity() << endl;
  
  for( const Pin &pin : net.pins() ) out << pin << " ";
  out << endl;
  
  for( const Point &p : net.path() ) out << p << " ";
  out << endl;

  return out;
}
