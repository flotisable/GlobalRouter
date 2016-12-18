#ifndef ROUTER_H
#define ROUTER_H

#include <vector>
#include <string>
using std::vector;
using std::string;

#include "RoutingGraph/RoutingGraph.h"
#include "MazeRouter.h"

class Router
{
  public:

    using RoutingEngine = MazeRouter;

    inline Router( RoutingEngine* router = nullptr );

    inline RoutingEngine* router() const;

    inline void setRouter   ( RoutingEngine*  router    );
    inline void setMaxLayer ( int             maxLayer  );

    bool readBlock( const string &fileName , const string &groupFileName );
    bool readNets ( const string &fileName );
    bool route    ();
    
    void outputData( const string &fileName ) const;

  private:

    const double unit = 1; // 0.01u

    bool readGroup( const string &fileName );

    vector<RoutingRegion*>  getRegions();
    void                    initRouter( const RoutingRegion *region , int maxLayer );
    bool                    netRouted ( const Net &net , const RoutingRegion *region );
    void                    saveNet   ( Net &net , RoutingRegion *region );

    RoutingEngine *mRouter;

    RoutingGraph  graph;
    int           maxLayer = 0;
};

inline Router::Router( RoutingEngine* router ) : mRouter( router ) {}

inline Router::RoutingEngine* Router::router() const { return mRouter; }

inline void Router::setRouter   ( RoutingEngine*  router    ) { mRouter         = router;   }
inline void Router::setMaxLayer ( int             maxLayer  ) { this->maxLayer  = maxLayer; }

#endif
