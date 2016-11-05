#ifndef NET_H
#define NET_H

#include <vector>
#include <string>
using std::vector;
using std::string;

#include "Graphic/Point.h"

class Net
{
  public:

    Net( const string &name = string() , int currentDensity );

    const string&  name          () const;
    const int      currentDensity() const;
    vector<Point>& connect       ();
    vector<Point>& mPath         ();

    void setName          ( const string &name );
    void setCurrentDensity( int );

  private:

    string mName;
    int    mCurrentDensity;

    vector<Point> mConnect;
    vector<Point> mPath;
};

#endif
