#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <ostream>

#include "Point.h"

class Rectangle
{
  public:

    Rectangle() = default;
    inline Rectangle( double centerX , double centerY , double h , double w );
    inline Rectangle( const Point &center             , double h , double w );

    inline const Point& center    () const;
    inline double       width     () const;
    inline double       height    () const;
    inline Point        leftButtom() const;

    inline void setCenter    ( double x , double y );
    inline void setCenter    ( const Point &p      );
    inline void setCenterX   ( double      x       );
    inline void setCenterY   ( double      y       );
    inline void setWidth     ( double      w       );
    inline void setHeight    ( double      h       );
    inline void setLeftButtom( double x , double y );

    inline double area  () const;
    inline double top   () const;
    inline double bottom() const;
    inline double left  () const;
    inline double right () const;

    inline Rectangle operator+( const Point &p ) const;

  private:

    Point   mCenter;
    double  mHeight;
    double  mWidth;
};

// Rectangle non-member function
std::ostream& operator<<( std::ostream &out , const Rectangle &rect );
// end Rectangle non-member function

// Rectangle inline member function
inline Rectangle::Rectangle(  double centerX  , double centerY , double h , double w )
  : Rectangle( Point( centerX , centerY ) , h , w ) {}
inline Rectangle::Rectangle( const Point &center , double h , double w )
  : mCenter( center ) , mHeight( h ) , mWidth( w ) {}

inline const Point& Rectangle::center    () const { return mCenter; }
inline double       Rectangle::width     () const { return mWidth; }
inline double       Rectangle::height    () const { return mHeight; }
inline Point        Rectangle::leftButtom() const { return Point( left() , bottom() );  }

inline void Rectangle::setCenter    ( double x , double y ) { mCenter  = Point( x , y ); }
inline void Rectangle::setCenter    ( const Point &p      ) { mCenter  = p; }
inline void Rectangle::setCenterX   ( double      x       ) { mCenter.setX( x ); }
inline void Rectangle::setCenterY   ( double      y       ) { mCenter.setY( y ); }
inline void Rectangle::setWidth     ( double      w       ) { mWidth   = w; }
inline void Rectangle::setHeight    ( double      h       ) { mHeight  = h; }
inline void Rectangle::setLeftButtom( double x , double y )
{ mCenter.set( x + mWidth / 2 , y + mHeight / 2 ); }

inline double Rectangle::area  () const { return mWidth       * mHeight;     }
inline double Rectangle::top   () const { return mCenter.y()  + mHeight / 2; }
inline double Rectangle::bottom() const { return mCenter.y()  - mHeight / 2; }
inline double Rectangle::left  () const { return mCenter.x()  - mWidth  / 2; }
inline double Rectangle::right () const { return mCenter.x()  + mWidth  / 2; }

inline Rectangle Rectangle::operator+( const Point &p ) const
{ return Rectangle( mCenter + p , mHeight , mWidth ); }
// end Rectangle inline member function

#endif
