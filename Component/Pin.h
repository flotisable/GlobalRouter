#ifndef PIN_H
#define PIN_H

#include "Point.h"

class Block;

class Pin : public Point
{
  public:
  
    inline Pin( Block* const connect = nullptr );
    inline Pin( Block* const connect , double x , double y );
    inline Pin( Block* const connect , const Point &p );
    
    inline Block* const connect();
    
    inline void setConnect( Block* const connect );
  
  private:
  
    Block *mConnect;
};

inline Pin( Block* const connect = nullptr ) : mConnect( connect ) {}
inline Pin( Block* const connect , double x , double y )
  : Point( x , y ) , mConnect( connect ) {}
inline Pin( Block* const connect , const Point &p )
  : Point( p ) , mConnect( connect ) {}

inline Block* const connect() { return mConnect; }

inline void setConnect( Block* const connect ) { mConnect = connect; }

#endif
