#include "Point.h"

#include <iomanip>
#include <string>

// Point non-member function
std::ostream& operator<<( std::ostream &out , const Point &point )
{
  const int &TAB = out.precision();

  return out  << std::right
              << "( " << std::setw( TAB ) << point.x() << " , "
                      << std::setw( TAB ) << point.y() << " )";
}

std::istream& operator>>( std::istream &in  , Point &point )
{
  std::string buffer;
  double x;
  double y;

  in >> buffer >> x >> buffer >> y >> buffer;
  
  point.set( x , y );
  
  return in;
}
// end Point non-member function
