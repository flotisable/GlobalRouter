#ifndef ROUTER_H
#define ROUTER_H

#include <vector>
#include <string>
#include <stdexcept>
using std::vector;
using std::string;

#include "RoutingGraph/RoutingGraph.h"
#include "MazeRouter.h"

// for exception
class FileOpenError : public std::runtime_error
{
  public:

    explicit FileOpenError( const string &fileName ) : std::runtime_error( fileName ) {}
};

class NetCannotRoute {};
// end for exception

class Router
{
  public:

    using RoutingEngine = MazeRouter;

    inline explicit Router( RoutingEngine* router = nullptr );

    inline RoutingEngine* router() const;

    inline void setRouter   ( RoutingEngine*  router    );
    inline void setMaxLayer ( int             maxLayer  );

    void readBlock( const string &fileName , const string &groupFileName );
    void readNets ( const string &fileName );
    void route    ();
    void outputData( const string &fileName ) const;

  private:

    const double unit = 0.01; // 0.01u

    void readGroup( const string &fileName );

    vector<RoutingRegion*>  getRegions();
    void                    initRouter( const RoutingRegion *region , int maxLayer );
    vector<Point>           sortPins  ( vector<Point> pins );
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
