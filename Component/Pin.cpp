#include "Pin.h"

std::istream& operator>>( std::istream &in , Pin &pin )
{
  Block   *block = new Block;
  string  name;
  Point   point;
  
  in >> name >> point;

  block->setName( name );
  
  pin.setConnect( block );
  pin.set( point.x() , point.y() );

  return in;
}
