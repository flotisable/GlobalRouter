#ifndef BLOCK_H
#define BLOCK_H

#include <string>
using std::string;

#include "Graphic/Point.h"
#include "Graphic/Rectangle.h"

class Block : public Rectangle
{
  public:
  
    inline Block( const string &name = string() );
    inline Block( const string &name , double centerX , double centerY , double h , double w );
    inline Block( const string &name , const Point &center             , double h , double w );
    
    inline const string& name();
    
    inline void setName( const string &name );
    
  private:
  
    string mName;
};

inline Block( const string &name = string() ) : mName( name ) {}
inline Block( const string &name , double centerX , double centerY , double h , double w )
  : Rectangle( centerX , centerY , h ,w ) , mName( name ) {}
inline Block( const string &name , const Point &center             , double h , double w )
  : Rectangle( center , h , w ) , mName( name ) {}

inline const string& name() { return mName; }

inline void setName( const string &name ) { mName = name; }

#endif
