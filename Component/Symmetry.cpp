#include "Symmetry.h"

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
  using std::getline;

  string word;

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
        symmetry.blocks().push_back( block );
      }
      break;
    }
  }
  return in;
}


Block* Symmetry::getBlock( const string &name )
{
  for( Block &block : blocks() )
     if( block.name() == name ) return &block;

  return nullptr;
}

const Block* Symmetry::getBlock( const string &name ) const
{
  for( const Block &block : blocks() )
     if( block.name() == name ) return &block;

  return nullptr;
}
