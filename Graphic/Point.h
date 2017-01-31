#ifndef POINT_H
#define POINT_H

#include <iostream>
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

    inline Point& operator+=( const Point &p );
    inline Point& operator-=( const Point &p );

  private:

    double mX;
    double mY;
};

// Point non-member function
std::ostream& operator<<( std::ostream &out , const Point &point );
std::istream& operator>>( std::istream &in  , Point       &point );

inline Point operator+( Point a , Point b ) { return a += b; }
inline Point operator-( Point a , Point b ) { return a -= b; }

inline bool operator==( const Point &a , const Point &b )
{ return ( a.x() == b.x() && a.y() == b.y() ); }
inline bool operator!=( const Point &a , const Point &b )
{ return !( a == b ); }

inline double manhattanDistance( const Point &a , const Point &b )
{ return ( abs( a.x() - b.x() ) + abs( a.y() - b.y() ) ); }
// end Point non-member function

// Point inline member funcion
inline Point::Point( double x , double y ) : mX( x ) , mY( y ) {}

inline double Point::x() const { return mX; }
inline double Point::y() const { return mY; }

inline void Point::setX( double x )             { mX = x; }
inline void Point::setY( double y )             { mY = y; }
inline void Point::set ( double x , double y )  { mX = x; mY = y; }

inline Point& Point::operator+=( const Point &p )
{ mX += p.x(); mY += p.y(); return *this; }
inline Point& Point::operator-=( const Point &p )
{ mX -= p.x(); mY -= p.y(); return *this; }
// end Point inline member funcion

#endif
