#ifndef ROUTER_H
#define ROUTER_H

#include <vector>
#include <string>
using std::vector;
using std::string;

#include "Component/Net.h"
#include "Component/Block.h"
#include "MazeRouter.h"

using RoutingEngine = MazeRouter;

class Router
{
  public:

    inline Router( RoutingEngine* const router = nullptr );

    inline RoutingEngine* const router() const;

    inline void setRouter( RoutingEngine* const router );

    bool readBlock( const string &fileName );
    bool readNets ( const string &fileName );
    bool route    ();
    
    void outputData( const string &fileName );

  private:

    RoutingEngine *mRouter;

    vector<double>  mHsplit;
    vector<double>  mVsplit;
    vector<Net>     nets;
    vector<Block>   blocks;
};

inline Router::Router( RoutingEngine* const router ) : mRouter( router ) {}

inline RoutingEngine* const Router::router() const { return mRouter; }

inline void Router::setRouter( RoutingEngine* const router ) { mRouter = router; }

#endif
