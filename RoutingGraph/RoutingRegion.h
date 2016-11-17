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
  
    inline const vector<double>&  hsplit() const;
    inline const vector<double>&  vsplit() const;
    inline vector<Block>&         blocks();
    
    virtual vector<vector<Grid>>  gridMap() = 0;
    vector<Point>                 connectedPin( Net &net );

    bool            netConnected( Net &net );
    virtual Block*  getBlock    ( const string &name );

    virtual void  buildSplit() = 0;
    
  protected:
  
    int getIndex( const vector<double> &array , double value );

    vector<double>  mHsplit;
    vector<double>  mVsplit;
    vector<Block>   mBlocks;
};

inline const vector<double>&  RoutingRegion::hsplit() { return mHsplit; }
inline const vector<double>&  RoutingRegion::vsplit() { return mVsplit; }
inline vector<Block>&         RoutingRegion::blocks() { return mBlocks; }

#endif
