#ifndef ROUTING_REGION_H
#define ROUTING_REGION_H

#include <vector>
#include <ostream>
using std::vector;

#include "../Component/Block.h"
#include "../Component/Net.h"
#include "../Component/Grid.h"
#include "../Graphic/Point.h"

class RoutingRegion : public Block
{
  public:

    virtual ~RoutingRegion() = default;

    inline vector<double>&        hsplit();
    inline const vector<double>&  hsplit() const;
    inline vector<double>&        vsplit();
    inline const vector<double>&  vsplit() const;
    inline vector<Block>&         blocks();
    inline const vector<Block>&   blocks() const;
    
    virtual vector<vector<Grid>>  gridMap() const;
    double                        maxGridWidth  () const;
    double                        maxGridHeight () const;
    virtual vector<Point>         connectedPin( const Net &net ) const;

    bool                  netConnected( Net &net ) const;
    virtual Block*        getBlock    ( const string &name );
    virtual const Block*  getBlock    ( const string &name ) const;

    virtual void  buildSplit() = 0;
    
  protected:
  
    int getIndex( const vector<double> &array , double value ) const;

    vector<double>  mHsplit;
    vector<double>  mVsplit;
    vector<Block>   mBlocks;
};

inline vector<double>&        RoutingRegion::hsplit()       { return mHsplit; }
inline const vector<double>&  RoutingRegion::hsplit() const { return mHsplit; }
inline vector<double>&        RoutingRegion::vsplit()       { return mVsplit; }
inline const vector<double>&  RoutingRegion::vsplit() const { return mVsplit; }
inline vector<Block>&         RoutingRegion::blocks()       { return mBlocks; }
inline const vector<Block>&   RoutingRegion::blocks() const { return mBlocks; }

#endif
