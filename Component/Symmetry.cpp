#include "Symmetry.h"

#include <cstdlib>
using namespace std;

std::ostream& operator<<( std::ostream &out , const Symmetry &symmetry )
{
  using std::endl;

  out << "[ Symmetry : " << symmetry.name() << " ]\n";

  out << "Blocks : " << symmetry.blocks().size() << endl;
  for( unsigned int i = 0 ; i < symmetry.blocks().size() ; ++i )
     out << symmetry.blocks()[i] << endl;

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
      int blockNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

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
  for( unsigned int i = 0 ; i < blocks().size() ; ++i )
     if( blocks()[i].name() == name ) return &blocks()[i];

  return NULL;
}

const Block* Symmetry::getBlock( const string &name ) const
{
  for( unsigned int i = 0 ; i < blocks().size() ; ++i )
     if( blocks()[i].name() == name ) return &blocks()[i];

  return NULL;
}
