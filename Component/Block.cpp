#include "Block.h"

std::ostream& operator<<( std::ostream &out , const Block &block )
{
  out << block.name() << " ";
  out << block.leftBottom() << " " << block.width() << " " << block.height();
  return out;
}

std::istream& operator>>( std::istream &in , Block &block )
{
  string name;
  string buffer;
  double lbX;
  double lbY;
  double width;
  double height;

  in >> name >> buffer >> lbX >> buffer >> lbY >> buffer >> width >> height;

  block.setName       ( name );
  block.setLeftBottom ( lbX , lbY );
  block.setWidth      ( width );
  block.setHeight     ( height );

  return in;
}
