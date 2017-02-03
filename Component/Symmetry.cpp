#include "Symmetry.h"

// Symmetry non-member functions
std::ostream& operator<<( std::ostream &out , const Symmetry &symmetry )
{
  using std::endl;

  out << "[ Symmetry : " << symmetry.name() << " ]\n";

  out << "Blocks : " << symmetry.blocks().size() << endl;
  for( const Block &block : symmetry.blocks() ) out << block << endl;

  return out;
}

std::istream& operator>>( std::istream &in  , Symmetry &symmetry )
{
  string word;

  while( !in.eof() )
  {
    std::getline( in , word );

    if( word.find( "Blocks : " ) != string::npos )
    {
      int blockNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

      for( int i = 0 ; i < blockNum ; ++i )
      {
        Block block;

        in >> block;
        symmetry.blocks().push_back( block );
      }
      break;
    }
  }
  return in;
}
// end Symmetry non-member functions

// Symmetry member functions
Block* Symmetry::getBlock( const string &name )
{
  for( Block &block : blocks() )
     if( block.name() == name ) return &block;

  return nullptr;
}

const Block* Symmetry::getBlock( const string &name ) const
{
  return const_cast<Symmetry*>( this )->getBlock( name );
}
// end Symmetry member functions