#ifndef SYMMETRY_H
#define SYMMETRY_H

#include <string>
#include <vector>
#include <ostream>
using std::string;
using std::vector;

#include "Block.h"

class Symmetry
{
  public:
  
    inline Symmetry( const string &name = string() );
  
    inline const string&  name();
    inline vector<Block>& blocks();
    
    inline void setName( const string &name );
    
    Block* getBlock( const string &name );
  
  private:
  
    string        mName;
    vector<Block> mBlocks;
};

std::ostream& operator<<( std::ostream &out , Symmetry &symmetry );

inline Symmetry::Symmetry( const string &name ) : mName( name ) {}

inline const string&  Symmetry::name  () { return mName;    }
inline vector<Block>& Symmetry::blocks() { return mBlocks;  }

inline void Symmetry::setName( const string &name ) { mName = name; }

#endif
