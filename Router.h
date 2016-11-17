#ifndef ROUTER_H
#define ROUTER_H

#include <vector>
#include <string>
using std::vector;
using std::string;

#include "Component/Net.h"
#include "Component/Block.h"
#include "Component/Group.h"
#include "MazeRouter.h"

using RoutingEngine = MazeRouter;

class Router : public Block
{
  public:

    inline Router( RoutingEngine* const router = nullptr );

    inline RoutingEngine* router() const;

    inline void setRouter( RoutingEngine* const router );

    bool readBlock( const string &fileName , const string &groupFileName );
    bool readNets ( const string &fileName );
    bool route    ();
    
    void outputData( const string &fileName );

  private:

    bool readGroup( const string &fileName );
    
    Block*  getBlock( const vector<Block>   &blocks , const string &name );
    int     getIndex( const vector<double>  &array  , double value );
    
    vector<vector<Grid>>  gridMap();
    vector<Point>         connectedPin( Net &net );

    RoutingEngine *mRouter;

    vector<double>  mHsplit;
    vector<double>  mVsplit;
    vector<Net>     nets;
    vector<Group>   groups;
    vector<Block>   blocks;
};

inline Router::Router( RoutingEngine* const router ) : mRouter( router ) {}

inline RoutingEngine* Router::router() const { return mRouter; }

inline void Router::setRouter( RoutingEngine* const router ) { mRouter = router; }

#endif
