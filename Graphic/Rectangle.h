#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <ostream>

#include "Point.h"

class Rectangle
{
  public:

    Rectangle() = default;
    inline Rectangle( double lbX , double lbY , double h , double w );
    inline Rectangle( const Point &lb         , double h , double w );

    inline const Point& leftBottom() const;
    inline const Point& rightTop  () const;
    inline Point        center    () const;
    inline double       width     () const;
    inline double       height    () const;

    inline void setLeftBottom( double x , double y );
    inline void setLeftBottom( const Point &p      );
    inline void setRightTop  ( double x , double y );
    inline void setRightTop  ( const Point &p      );
    inline void setCenter    ( double x , double y );
    inline void setCenter    ( const Point &p      );
    inline void setCenterX   ( double      x       );
    inline void setCenterY   ( double      y       );
    inline void setWidth     ( double      w       );
    inline void setHeight    ( double      h       );

    inline double area  () const;
    inline double top   () const;
    inline double bottom() const;
    inline double left  () const;
    inline double right () const;
    
    inline void setTop    ( double top    );
    inline void setBottom ( double bottom );
    inline void setLeft   ( double left   );
    inline void setRight  ( double right  );

    inline Rectangle operator+( const Point &p ) const;

  private:

    Point lb;
    Point rt;
};

// Rectangle non-member function
std::ostream& operator<<( std::ostream &out , const Rectangle &rect );
// end Rectangle non-member function

// Rectangle inline member function
inline Rectangle::Rectangle(  double lbX  , double lbY , double h , double w )
  : Rectangle( Point( lbX , lbY ) , h , w ) {}
inline Rectangle::Rectangle( const Point &lb , double h , double w )
  : lb( lb ) , rt( lb.x() + w , lb.y() + h ) {}

inline const Point& Rectangle::leftBottom() const { return lb; }
inline const Point& Rectangle::rightTop  () const { return rt; }
inline Point        Rectangle::center    () const { return Point( ( lb.x() + rt.x() ) / 2 ,
                                                                  ( lb.y() + rt.y() ) / 2 ); }
inline double       Rectangle::width     () const { return rt.x() - lb.x(); }
inline double       Rectangle::height    () const { return rt.y() - lb.y(); }

inline void Rectangle::setLeftBottom( double x , double y ) { lb = Point( x , y ); }
inline void Rectangle::setLeftBottom( const Point &p      ) { lb = p; }
inline void Rectangle::setRightTop  ( double x , double y ) { rt = Point( x , y ); }
inline void Rectangle::setRightTop  ( const Point &p      ) { rt = p; }
inline void Rectangle::setCenter    ( double x , double y )
{ rt = Point( lb.x() + 2 * x , lb.y() + 2 * y ); }
inline void Rectangle::setCenter    ( const Point &p      )
{ rt = Point( lb.x() + 2 * p.x() , lb.x() + 2 * p.y() ); }
inline void Rectangle::setCenterX   ( double      x       ) { rt.setX( lb.x() + 2 * x ); }
inline void Rectangle::setCenterY   ( double      y       ) { rt.setY( lb.y() + 2 * y ); }
inline void Rectangle::setWidth     ( double      w       ) { rt.setX( lb.x() + w ); }
inline void Rectangle::setHeight    ( double      h       ) { rt.setY( lb.y() + h ); }

inline double Rectangle::area  () const { return width() * height(); }
inline double Rectangle::top   () const { return rt.y(); }
inline double Rectangle::bottom() const { return lb.y(); }
inline double Rectangle::left  () const { return lb.x(); }
inline double Rectangle::right () const { return rt.x(); }

inline void Rectangle::setTop    ( double top     ) { rt.setY( top    ); }
inline void Rectangle::setBottom ( double bottom  ) { lb.setY( bottom ); }
inline void Rectangle::setLeft   ( double left    ) { lb.setX( left   ); }
inline void Rectangle::setRight  ( double right   ) { rt.setX( right  ); }

inline Rectangle Rectangle::operator+( const Point &p ) const
{ return Rectangle( center() + p , height() , width() ); }
// end Rectangle inline member function

#endif
