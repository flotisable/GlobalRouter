#ifndef NET_H
#define NET_H

#include <vector>
#include <string>
using std::vector;
using std::string;

#include "../Graphic/Point.h"
#include "Pin.h"

class Net
{
  public:

    Net( const string &name );
    Net( const string &name , int currentDensity );

    const string&   name          () const;
    const int       currentDensity() const;
    vector<Pin>&    pin           ();
    vector<Point>&  path          ();

    void setName          ( const string &name );
    void setCurrentDensity( int );

  private:

    string mName;
    int    mCurrentDensity;

    vector<Pin>   mPin;
    vector<Point> mPath;
};

#endif
