#include "Symmetry.h"

std::ostream& operator<<( std::ostream &out , Symmetry &symmetry )
{
  using std::endl;

  out << "[ Symmetry : " << symmetry.name() << " ]\n";

  out << "Blocks : " << symmetry.blocks().size() << endl;
  for( const Block &block : symmetry.blocks() ) out << block << endl;

  return out;
}
