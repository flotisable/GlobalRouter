#ifndef GROUP_H
#define GROUP_H

#include <vector>
#include <ostream>
using std::vector;

#include "../RoutingGraph/RoutingRegion.h"
#include "Symmetry.h"

class Group : public RoutingRegion
{
  public:

    inline vector<Symmetry>& symmetrys();
    
    virtual vector<vector<Grid>> gridMap() override;
    
    virtual Block* getBlock( const string &name ) override;

    virtual void buildSplit() override;

  private:

    vector<Symmetry>  mSymmetrys;
};

std::ostream& operator<<( std::ostream &out , Group &group );

inline vector<Symmetry>& Group::symmetrys() { return mSymmetrys; }

#endif
