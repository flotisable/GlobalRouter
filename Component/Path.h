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

    inline Path( RoutingRegion *region = NULL );

    inline RoutingRegion*       belongRegion() const;
    inline vector<Point>&       path        ();
    inline const vector<Point>& path        () const;

    inline void setBelongRegion( RoutingRegion *region );
  
  private:

    RoutingRegion *mBelongRegion;
    vector<Point> mPath;
};

std::ostream& operator<<( std::ostream &out , const Path &path );
std::istream& operator>>( std::istream &in  , Path &path );

inline Path::Path( RoutingRegion *region ) : mBelongRegion( region ) {}

inline RoutingRegion*       Path::belongRegion() const  { return mBelongRegion; }
inline vector<Point>&       Path::path        ()        { return mPath; }
inline const vector<Point>& Path::path        () const  { return mPath; }

inline void Path::setBelongRegion( RoutingRegion *region )
{ mBelongRegion = region; }

#endif
