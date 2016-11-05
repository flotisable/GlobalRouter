#ifndef NET_H
#define NET_H

#include <vector>
#include <string>
using std::vector;
using std::string;

#include "../Graphic/Point.h"

class Net
{
  public:

    Net( const string &name );
    Net( const string &name , int currentDensity );

    const string&  name          () const;
    const int      currentDensity() const;
    vector<Point>& connect       ();
    vector<Point>& path          ();

    void setName          ( const string &name );
    void setCurrentDensity( int );

  private:

    string mName;
    int    mCurrentDensity;

    vector<Point> mConnect;
    vector<Point> mPath;
};

#endif
