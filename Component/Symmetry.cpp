#include "Symmetry.h"

#include <algorithm>

std::ostream& operator<<( std::ostream &out , Symmetry &symmetry )
{
  using std::endl;

  out << "[ Symmetry : " << symmetry.name() << " ]\n";

  out << "Blocks : " << symmetry.blocks().size() << endl;
  for( const Block &block : symmetry.blocks() ) out << block << endl;

  return out;
}

Block* Symmetry::getBlock( const string &name )
{
  auto it = std::find_if( mBlocks.begin() , mBlocks.end() ,
                          [&]( const Block &block ) { return block.name() == name; } );

  return ( it != mBlocks.end() ) ? const_cast<Block* const>( &( *it ) ) : nullptr;
}
