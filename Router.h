#ifndef ROUTER_H
#define ROUTER_H

#include <vector>
#include <string>
#include <stdexcept>
using std::vector;
using std::string;

#include "RoutingGraph/RoutingGraph.h"
#include "MazeRouter.h"

class Router
{
  public:

    typedef MazeRouter RoutingEngine;

    // exception classes
    class FileOpenError;
    class RoutingEngineError;
    class NetCannotRoute {};
    // end exception classes

    inline explicit Router( RoutingEngine* router = NULL );

    inline RoutingEngine* router() const;

    inline void setRouter   ( RoutingEngine*  router    );
    inline void setMaxLayer ( int             maxLayer  );

    void read       ( const string &groupFileName , const string &blockFileName , const string &netFileName );
    void route      ();
    void outputData ( const string &fileName ) const;

  private:

    const double unit; // unit u

    void readGroup( const string &fileName );
    void readBlock( const string &fileName );
    void readNets ( const string &fileName );

    void initRouter( const RoutingRegion *region , int maxLayer );

    RoutingEngine *mRouter;
    RoutingGraph  graph;
    int           maxLayer;
};

// exception classes
class Router::FileOpenError : public std::runtime_error
{
  public:

    explicit FileOpenError( const string &fileName ) : std::runtime_error( fileName ) {}
};

class Router::RoutingEngineError
{
  public:

    inline RoutingEngineError( const string &engineName , const string &errorName );

    inline const string& engineName() const;
    inline const string& errorName () const;

  private:

    const string engine;
    const string error;
};
// end exception classes

// Router non-member functions
vector<const RoutingRegion*>  getRegions( const RoutingGraph &graph );
vector<Point>                 movePins  ( vector<Point> pins , const RoutingRegion *region , const Net &net );
vector<Point>                 sortPins  ( vector<Point> pins );
bool                          netRouted ( const Net &net , const RoutingRegion *region );
void                          saveNet   ( Net &net , RoutingRegion *region , vector<Path> &paths );
// end Router non-member functions

// Router public inline member functions
inline Router::Router( RoutingEngine* router )
  : unit( 0.01 ) , mRouter( router ) , maxLayer( 0 ) {}

inline Router::RoutingEngine* Router::router() const { return mRouter; }

inline void Router::setRouter   ( RoutingEngine*  router    ) { mRouter         = router;   }
inline void Router::setMaxLayer ( int             maxLayer  )
{ if( maxLayer < 0 ) throw std::invalid_argument( "Router maxlayer < 0" ); this->maxLayer  = maxLayer; }
// end Router public inline member functions

// Router::RoutingEngineError public inline member functions
inline Router::RoutingEngineError::RoutingEngineError( const string &engineName , const string &errorName )
  : engine( engineName ) , error( errorName ) {}

inline const string& Router::RoutingEngineError::engineName () const { return engine; }
inline const string& Router::RoutingEngineError::errorName  () const { return error;  }
// end Router::RoutingEngineError public inline member functions

#endif
