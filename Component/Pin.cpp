#include "Pin.h"
#include "Block.h"

std::ostream& operator<<( std::ostream &out , const Pin &pin )
{
  return out << pin.connect()->name() << static_cast<Point>( pin );
}
