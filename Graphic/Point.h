#ifndef POINT_H
#define POINT_H

#include <ostream>
#include <cmath>

class Point
{
  public:

    Point() = default;
    inline Point( double x , double y );

    inline double x() const;
    inline double y() const;

    inline void setX( double x );
    inline void setY( double y );
    inline void set ( double x , double y );

    inline Point  operator+ ( const Point &p ) const;
    inline Point  operator- ( const Point &p ) const;
    inline Point& operator+=( const Point &p );
    inline Point& operator-=( const Point &p );

    inline bool operator==( const Point &p ) const;
    inline bool operator!=( const Point &p ) const;

  private:

    double mX;
    double mY;
};

// Point non-member function
std::ostream& operator<<( std::ostream &out , const Point &point );

inline double manhattanDistance( const Point &a , const Point &b )
{ return ( abs( a.x() - b.x() ) + abs( a.y() - b.y() ) ); }
// end Point non-member function

// Point inline member funcion
inline Point::Point( double x , double y ) { set( x , y ); }

inline double Point::x() const { return mX; }
inline double Point::y() const { return mY; }

inline void Point::setX( double x )                   { mX = x; }
inline void Point::setY( double y )                   { mY = y; }
inline void Point::set ( double x , const double y )  { mX = x; mY = y; }

inline Point Point::operator+( const Point &p ) const
{ return Point( mX + p.x() , mY + p.y() ); }
inline Point Point::operator-( const Point &p ) const
{ return Point( mX - p.x() , mY - p.y() ); }
inline Point& Point::operator+=( const Point &p )
{ return *this = *this + p; }
inline Point& Point::operator-=( const Point &p )
{ return *this = *this - p; }

inline bool Point::operator==( const Point &p ) const
{ return ( mX == p.x() && mY == p.y() ); }
inline bool Point::operator!=( const Point &p ) const
{ return ( mX != p.x() || mY != p.y() ); }
// end Point inline member funcion

#endif
