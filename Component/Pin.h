#ifndef PIN_H
#define PIN_H

#include <iostream>

#include "../Graphic/Point.h"
#include "Block.h"

class Pin : public Point
{
  public:

    inline explicit Pin( Block *connect = nullptr );
    inline Pin( Block *connect , double x , double y );
    inline Pin( Block *connect , const Point &p );

    inline Block* connect() const;

    inline void setConnect( Block *connect );
  
  private:

    Block *mConnect;
};

// Pin non-member functions
inline std::ostream& operator<<( std::ostream &out , const Pin &pin )
{ return out << pin.connect()->name() << " " << static_cast<Point>( pin ); }
std::istream& operator>>( std::istream &in , Pin &pin );
// end Pin non-member functions

// Pin inline member functions
inline Pin::Pin( Block *connect ) : mConnect( connect ) {}
inline Pin::Pin( Block *connect , double x , double y )
  : Point{ x , y } , mConnect{ connect } {}
inline Pin::Pin( Block *connect , const Point &p )
  : Pin{ connect , p.x() , p.y() } {}

inline Block* Pin::connect() const { return mConnect; }

inline void Pin::setConnect( Block *connect ) { mConnect = connect; }
// end Pin inline member functions

#endif
