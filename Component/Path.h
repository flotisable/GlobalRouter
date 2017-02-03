#ifndef PATH_H
#define PATH_H

#include <vector>
#include <iostream>
#include <stdexcept>
using std::vector;

#include "../Graphic/Point.h"

class RoutingRegion;

class Path
{
  public:

    inline explicit Path( int layer = 0 );

    inline RoutingRegion*       belongRegion() const;
    inline vector<Point>&       path        ();
    inline const vector<Point>& path        () const;
    inline int                  layer       () const;

    inline void setBelongRegion ( RoutingRegion *region );
    inline void setLayer        ( int           layer   );

  private:

    RoutingRegion *mBelongRegion = nullptr;
    vector<Point> mPath;
    int           mLayer;
};

// Path non-member functions
std::ostream& operator<<( std::ostream &out , const Path &path );
std::istream& operator>>( std::istream &in  , Path &path );
// end Path non-member functions

// Path inline member functions
inline Path::Path( int layer ) { setLayer( layer ); }

inline RoutingRegion*       Path::belongRegion() const  { return mBelongRegion; }
inline vector<Point>&       Path::path        ()        { return mPath;         }
inline const vector<Point>& Path::path        () const  { return mPath;         }
inline int                  Path::layer       () const  { return mLayer;        }

inline void Path::setBelongRegion ( RoutingRegion *region ) { mBelongRegion = region; }
inline void Path::setLayer        ( int           layer   )
{ if( layer < 0 ) throw std::invalid_argument{ "Path layer < 0" }; mLayer  = layer;  }
// end Path inline member functions

#endif
