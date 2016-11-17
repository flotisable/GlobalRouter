#include "Net.h"

std::ostream& operator<<( std::ostream &out , Net &net )
{
  using std::endl;

  out << net.name() << " " << net.currentDensity() << " "
      << net.pins().size() << " "<< net.paths().size() << endl;
  
  for( const Pin &pin : net.pins() ) out << pin << " ";
  out << endl;
  
  for( Path &path : net.paths() ) out << path << endl;

  return out;
}
