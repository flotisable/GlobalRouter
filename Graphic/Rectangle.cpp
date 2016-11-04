#include "Rectangle.h"

#include <iomanip>

// Rectangle non-member function
std::ostream& operator<<( std::ostream &out , const Rectangle &rect )
{
  const int &TAB = out.precision();

  return out  << std::right       << rect.center() << " "
              << std::setw( TAB ) << rect.height() << " "
              << std::setw( TAB ) << rect.width ();
}
// Rectangle non-member function
