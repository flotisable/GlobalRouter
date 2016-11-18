#ifndef ROUTING_GRAPH_H
#define ROUTING_GRAPH_H

#include <iostream>
#include <vector>
using std::vector;

#include "RoutingRegion.h"
#include "../Component/Grid.h"
#include "../Component/Net.h"
#include "../Component/Group.h"

class RoutingGraph : public RoutingRegion
{
  public:

    inline vector<Net>&   nets  ();
    inline vector<Group>& groups();

    virtual vector<vector<Grid>>  gridMap   () override;
    virtual void                  buildSplit() override;

    virtual Block* getBlock ( const string &name ) override;
    RoutingRegion* getRegion( const string &name );
    
    Block operator=( const Block &block );
  
  private:
  
    vector<Net>   mNets;
    vector<Group> mGroups;
};

std::ostream& operator<<( std::ostream &out , RoutingGraph &graph );
std::istream& operator>>( std::istream &in  , RoutingGraph &graph );

inline vector<Net>&   RoutingGraph::nets  () { return mNets;    }
inline vector<Group>& RoutingGraph::groups() { return mGroups;  }

#endif
