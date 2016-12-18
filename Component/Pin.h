#ifndef PIN_H
#define PIN_H

#include <iostream>

#include "../Graphic/Point.h"
#include "Block.h"

class Pin : public Point
{
  public:
  
    inline Pin( Block* const connect = NULL );
    inline Pin( Block* const connect , double x , double y );
    inline Pin( Block* const connect , const Point &p );
    
    inline Block* connect() const;
    
    inline void setConnect( Block* const connect );
  
  private:
  
    Block *mConnect;
};

inline std::ostream& operator<<( std::ostream &out , const Pin &pin )
{ return out << pin.connect()->name() << " " << static_cast<Point>( pin ); }
std::istream& operator>>( std::istream &in , Pin &pin );

inline Pin::Pin( Block* const connect ) : mConnect( connect ) {}
inline Pin::Pin( Block* const connect , double x , double y )
  : Point( x , y ) , mConnect( connect ) {}
inline Pin::Pin( Block* const connect , const Point &p )
  : Point( p ) , mConnect( connect ) {}

inline Block* Pin::connect() const { return mConnect; }

inline void Pin::setConnect( Block* const connect ) { mConnect = connect; }

#endif
