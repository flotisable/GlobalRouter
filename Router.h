#ifndef ROUTER_H
#define ROUTER_H

#include <vector>
#include <string>
using std::vector;
using std::string;

#include "RoutingGraph/RoutingRegion.h"
#include "Component/Net.h"
#include "Component/Block.h"
#include "Component/Group.h"
#include "MazeRouter.h"

using RoutingEngine = MazeRouter;

class Router : public RoutingRegion
{
  public:

    inline Router( RoutingEngine* router = nullptr );

    inline RoutingEngine* router() const;

    inline void setRouter( RoutingEngine* router );

    bool readBlock( const string &fileName , const string &groupFileName );
    bool readNets ( const string &fileName );
    bool route    ();
    
    void outputData( const string &fileName );
    
    virtual vector<vector<Grid>>  gridMap   () override;
    virtual void                  buildSplit() override;
    
    virtual Block* getBlock( const string &name ) override;

  private:

    bool readGroup( const string &fileName );

    RoutingEngine *mRouter;

    vector<Net>   nets;
    vector<Group> groups;
};

inline Router::Router( RoutingEngine* router ) : mRouter( router ) {}

inline RoutingEngine* Router::router() const { return mRouter; }

inline void Router::setRouter( RoutingEngine* router ) { mRouter = router; }

#endif
