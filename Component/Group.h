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
    
    virtual vector<vector<Grid>> gridMap() const override;
    
    virtual Block*        getBlock( const string &name ) override;
    virtual const Block*  getBlock( const string &name ) const override;

    virtual void buildSplit() override;

  private:

    vector<Symmetry>  mSymmetrys;
};

std::ostream& operator<<( std::ostream &out , const Group &group );
std::istream& operator>>( std::istream &in  , Group       &group );

inline vector<Symmetry>&        Group::symmetrys()        { return mSymmetrys; }
inline const vector<Symmetry>&  Group::symmetrys() const  { return mSymmetrys; }

#endif
