#include "Group.h"

std::ostream& operator<<( std::ostream &out , Group &group )
{
  using std::endl;

  out << "[ Group : " << group.name() << "]\n";

  out << "Symmetrys :\n";
  for( Symmetry& symmetry : group.symmetrys() ) out << symmetry << endl;
  
  out << "Blocks :\n";
  for( const Block& block : group.blocks() ) out << block << endl;

  return out;
}
