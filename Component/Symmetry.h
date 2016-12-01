#ifndef SYMMETRY_H
#define SYMMETRY_H

#include <string>
#include <vector>
#include <iostream>
using std::string;
using std::vector;

#include "Block.h"

class Symmetry
{
  public:
  
    inline Symmetry( const string &name = string() );
  
    inline const string&        name  () const;
    inline vector<Block>&       blocks();
    inline const vector<Block>& blocks() const;
    
    inline void setName( const string &name );
    
    Block* getBlock( const string &name ) const;
  
  private:
  
    string        mName;
    vector<Block> mBlocks;
};

std::ostream& operator<<( std::ostream &out , const Symmetry  &symmetry );
std::istream& operator>>( std::istream &in  , Symmetry        &symmetry );

inline Symmetry::Symmetry( const string &name ) : mName( name ) {}

inline const string&        Symmetry::name  () const  { return mName;    }
inline vector<Block>&       Symmetry::blocks()        { return mBlocks;  }
inline const vector<Block>& Symmetry::blocks() const  { return mBlocks;  }

inline void Symmetry::setName( const string &name ) { mName = name; }

#endif
