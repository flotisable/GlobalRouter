#include "Block.h"

std::ostream& operator<<( std::ostream &out , const Block &block )
{
  out << block.name() << " ";
  out << block.leftBottom() << " " << block.width() << " " << block.height();
  return out;
}
