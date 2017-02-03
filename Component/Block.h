#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <iostream>
using std::string;

#include "../Graphic/Rectangle.h"

class Block : public Rectangle
{
  public:

    enum class Type
    {
      block,
      region
    };

    inline Block( const string &name = string() , Type type = Type::block );
    
    inline const string&  name() const;
    inline Type           type() const;
    
    inline void setName( const string &name );
    inline void setType( Type         type  );

  private:

    string  mName;
    Type    mType;
};

// Block non-member functions
std::ostream& operator<<( std::ostream &out , const Block &block );
std::istream& operator>>( std::istream &in  , Block &block );
// end Block non-member functions

// Block inline member functions
inline Block::Block( const string &name , Type type ) : mName( name ) , mType( type ) {}

inline const string&  Block::name() const { return mName; }
inline Block::Type    Block::type() const { return mType; }

inline void Block::setName( const string  &name ) { mName = name; }
inline void Block::setType( Type          type  ) { mType = type; }
// end Block inline member functions

#endif
