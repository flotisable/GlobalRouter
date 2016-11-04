#ifndef ROUTER_H
#define ROUTER_H

#include <vector>
#include <string>
using std::vector;
using std::string;

#include "Net.h"
#include "Graphic/Rectangle.h"
#include "MazeRouter.h"

using RoutingEngine = MazeRouter;

class Router
{
  public:

    Router( RoutingEngine* const router = nullptr );

    RoutingEngine* const router();
    vector<double>&      hsplit();
    vector<double>&      vsplit();

    void setRouter( RoutingEngine* const router );

    bool readBlock( const string &fileName );
    bool readNets ( const string &fileName );
    bool route    ();

  private:

    RoutingEngine *mRouter;

    vector<double>    mHsplit;
    vector<double>    mVsplit;
    vector<Net>       nets;
    vector<Rectangle> blocks;
};

#endif
