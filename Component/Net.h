#ifndef NET_H
#define NET_H

#include <vector>
#include <string>
#include <ostream>
using std::vector;
using std::string;

#include "../Graphic/Point.h"
#include "Pin.h"

class Net
{
  public:

    inline Net( const string &name = string() );
    inline Net( const string &name , int currentDensity );

    inline const string&  name          () const;
    inline int            currentDensity() const;
    inline vector<Pin>&   pins          ();
    inline vector<Point>& path          ();

    inline void setName          ( const string &name           );
    inline void setCurrentDensity( int          currentDensity  );

  private:

    string mName;
    int    mCurrentDensity;

    vector<Pin>   mPins;
    vector<Point> mPath;
};

std::ostream& operator<<( std::ostream &out , Net &net );

inline Net::Net( const string &name ) : mName( name ) {}
inline Net::Net( const string &name , int currentDensity )
  : mName( name ) , mCurrentDensity( currentDensity ) {}

inline const string&  Net::name          () const  { return mName;           }
inline int            Net::currentDensity() const  { return mCurrentDensity; }
inline vector<Pin>&   Net::pins          ()        { return mPins;           }
inline vector<Point>& Net::path          ()        { return mPath;           }

inline void Net::setName          ( const string  &name )
{ mName = name; }
inline void Net::setCurrentDensity( int           currentDensity )
{ mCurrentDensity = currentDensity; }

#endif
