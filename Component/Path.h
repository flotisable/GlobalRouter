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

    inline RoutingRegion*       belongRegion() const;
    inline vector<Point>&       path        ();
    inline const vector<Point>& path        () const;
    inline int                  layer       () const;

    inline void setBelongRegion ( RoutingRegion *region );
    inline void setLayer        ( int           layer   );

  private:

    RoutingRegion *mBelongRegion = nullptr;
    vector<Point> mPath;
    int           mLayer = 0;
};

std::ostream& operator<<( std::ostream &out , const Path &path );
std::istream& operator>>( std::istream &in  , Path &path );

inline RoutingRegion*       Path::belongRegion() const  { return mBelongRegion; }
inline vector<Point>&       Path::path        ()        { return mPath; }
inline const vector<Point>& Path::path        () const  { return mPath; }
inline int                  Path::layer       () const  { return mLayer; }

inline void Path::setBelongRegion ( RoutingRegion *region ) { mBelongRegion = region; }
inline void Path::setLayer        ( int           layer   ) { mLayer        = layer;  }

#endif
