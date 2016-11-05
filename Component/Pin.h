#ifndef PIN_H
#define PIN_H

#include "../Graphic/Point.h"

class Block;

class Pin : public Point
{
  public:
  
    inline Pin( Block* const connect = nullptr );
    inline Pin( Block* const connect , double x , double y );
    inline Pin( Block* const connect , const Point &p );
    
    inline Block* const connect() const;
    
    inline void setConnect( Block* const connect );
  
  private:
  
    Block *mConnect;
};

inline Pin::Pin( Block* const connect ) : mConnect( connect ) {}
inline Pin::Pin( Block* const connect , double x , double y )
  : Point( x , y ) , mConnect( connect ) {}
inline Pin::Pin( Block* const connect , const Point &p )
  : Point( p ) , mConnect( connect ) {}

inline Block* const Pin::connect() const { return mConnect; }

inline void Pin::setConnect( Block* const connect ) { mConnect = connect; }

#endif
