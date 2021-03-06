#ifndef NET_H
#define NET_H

#include <vector>
#include <string>
#include <iostream>
using std::vector;
using std::string;

#include "Pin.h"
#include "Path.h"

class Net
{
  public:

    inline explicit Net( const string &name = string() );
    inline Net( const string &name , int currentDensity );

    inline const string&        name          () const;
    inline int                  currentDensity() const;
    inline vector<Pin>&         pins          ();
    inline const vector<Pin>&   pins          () const;
    inline vector<Path>&        paths         ();
    inline const vector<Path>&  paths         () const;

    inline void setName          ( const string &name           );
    inline void setCurrentDensity( int          currentDensity  );

  private:

    string mName;
    int    mCurrentDensity;

    vector<Pin>   mPins;
    vector<Path>  mPaths;
};

// Net non-member functions
std::ostream& operator<<( std::ostream &out , const Net &net );
std::istream& operator>>( std::istream &in  , Net       &net );
// end Net non-member functions

// Net inline member functions
inline Net::Net( const string &name ) : mName{ name } {}
inline Net::Net( const string &name , int currentDensity )
  : mName{ name } , mCurrentDensity{ currentDensity } {}

inline const string&        Net::name           () const  { return mName;           }
inline int                  Net::currentDensity () const  { return mCurrentDensity; }
inline vector<Pin>&         Net::pins           ()        { return mPins;           }
inline const vector<Pin>&   Net::pins           () const  { return mPins;           }
inline vector<Path>&        Net::paths          ()        { return mPaths;          }
inline const vector<Path>&  Net::paths          () const  { return mPaths;          }

inline void Net::setName          ( const string  &name )
{ mName = name; }
inline void Net::setCurrentDensity( int currentDensity )
{ mCurrentDensity = currentDensity; }
// end Net inline member functions

#endif
