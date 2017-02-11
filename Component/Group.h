#ifndef GROUP_H
#define GROUP_H

#include <vector>
#include <iostream>
using std::vector;

#include "../RoutingGraph/RoutingRegion.h"
#include "Symmetry.h"

class Group : public RoutingRegion
{
  public:

    inline vector<Symmetry>& symmetrys();
    inline const vector<Symmetry>& symmetrys() const;

    virtual GridMap       gridMap( int layer = 1 ) const;
    virtual Block*        getBlock( const string &name );
    virtual const Block*  getBlock( const string &name ) const;
    virtual void          buildSplit();

  private:

    vector<Symmetry>  mSymmetrys;
};

// Group non-member functions
std::ostream& operator<<( std::ostream &out , const Group &group );
std::istream& operator>>( std::istream &in  , Group       &group );
// end Group non-member functions

// Group inline member functions
inline vector<Symmetry>&        Group::symmetrys()        { return mSymmetrys; }
inline const vector<Symmetry>&  Group::symmetrys() const  { return mSymmetrys; }
// end Group inline member functions

#endif
