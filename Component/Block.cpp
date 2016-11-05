#include "Block.h"

std::ostream& operator<<( std::ostream &out , const Block &block )
{
  out << block.name() << " ";
  out << block.leftButtom() << " " << block.width() << " " << block.height();
  return out;
}
