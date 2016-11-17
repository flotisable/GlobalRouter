#include "Path.h"

#include "Block.h"
#include "pin.h"

ostream& operator<<( ostream &out , Path &path )
{
  using std::endl;
  
  out << path.belongBlock()->name() << " " << path.path().size() << " ";
  
  for( const Point &p : path.path() ) out << p << " ";

  return out;
}
