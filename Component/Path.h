#ifndef PATH_H
#define PATH_H

#include <vector>
#include <iostream>
using std::vector;

#include "../Graphic/Point.h"

class RoutingRegion;

class Path
{
  public:

    inline RoutingRegion* belongRegion() const;
    inline vector<Point>& path        ();

    inline void setBelongRegion( RoutingRegion *region );
  
  private:

    RoutingRegion *mBelongRegion = nullptr;
    vector<Point> mPath;
};

std::ostream& operator<<( std::ostream &out , Path &path );
std::istream& operator>>( std::istream &in  , Path &path );

inline RoutingRegion* Path::belongRegion() const  { return mBelongRegion; }
inline vector<Point>& Path::path        ()        { return mPath; }

inline void Path::setBelongRegion( RoutingRegion *region )
{ mBelongRegion = region; }

#endif
