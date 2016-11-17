#ifndef PATH_H
#define PATH_H

#include <vector>
#include <ostream>
using std::vector;
using std::ostream;

#include "../Graphic/Point.h"

class Block;

class Path
{
  public:

    inline Block*         belongBlock () const;
    inline vector<Point>& path        ();

    inline void setBelongBlock( Block *block );
  
  private:

    Block         *mBelongBlock = nullptr;
    vector<Point> mPath;
};

ostream& operator<<( ostream &out , Path &path );

inline Block*         Path::belongBlock () const  { return mBelongBlock; }
inline vector<Point>& Path::path        ()        { return mPath; }

inline void Path::setBelongBlock( Block *block ) { mBelongBlock  = block; }

#endif
