#ifndef ROUTING_GRAPH_H
#define ROUTING_GRAPH_H

#include <iostream>
#include <vector>
using std::vector;

#include "RoutingRegion.h"
#include "../GridMap/GridMap.h"
#include "../Component/Net.h"
#include "../Component/Group.h"

class RoutingGraph : public RoutingRegion
{
  public:

    inline vector<Net>&         nets  ();
    inline const vector<Net>&   nets  () const;
    inline vector<Group>&       groups();
    inline const vector<Group>& groups() const;

    virtual GridMap gridMap   ( int layer = 1 ) const;
    virtual void    buildSplit();
    
    virtual vector<Point> connectedPin( const Net &net ) const;

    virtual Block*        getBlock  ( const string &name );
    virtual const Block*  getBlock  ( const string &name ) const;
    RoutingRegion*        getRegion ( const string &name );
    const RoutingRegion*  getRegion ( const string &name ) const;
    Net*                  getNet    ( const string &name );
    const Net*            getNet    ( const string &name ) const;

    const Block& operator=( const Block &block );

  private:
  
    vector<Net>   mNets;
    vector<Group> mGroups;
};

// RoutingGraph non-member functions
std::ostream& operator<<( std::ostream &out , const RoutingGraph  &graph );
std::istream& operator>>( std::istream &in  , RoutingGraph        &graph );
// end RoutingGraph non-member functions

// RoutingGraph inline member functions
inline vector<Net>&         RoutingGraph::nets  ()        { return mNets;    }
inline const vector<Net>&   RoutingGraph::nets  () const  { return mNets;    }
inline vector<Group>&       RoutingGraph::groups()        { return mGroups;  }
inline const vector<Group>& RoutingGraph::groups() const  { return mGroups;  }
// end RoutingGraph inline member functions

#endif
