#include "Point.h"

#include <iomanip>

// Point non-member function
std::ostream& operator<<( std::ostream &out , const Point &point )
{
  const int &TAB = out.precision();

  return out  << std::right
              << "( " << std::setw( TAB ) << point.x() << " , "
                      << std::setw( TAB ) << point.y() << " )";
}
// end Point non-member function
