#ifndef EDGE_H
#define EDGE_H

#include <vector>

class Edge
{
  public:

    typedef double CostType;

    inline explicit Edge( int layer = 1 , bool enabled = true );

    inline bool     enabled() const;
    inline int      layer  () const;
    inline CostType cost   ( int layer ) const;

    inline void setEnabled( bool  enabled );
    inline void setLayer  ( int   layer );
    inline void setCost   ( const CostType &cost , int layer );

  private:

    bool                  mEnabled;
    int                   mLayer;
    std::vector<CostType> mCost;
};

// Edge inline member functions
inline Edge::Edge( int layer, bool enabled ) : mEnabled( enabled ) , mLayer( 0 )
{ mCost.resize( layer , CostType() ); }

inline bool           Edge::enabled ()            const { return mEnabled;      }
inline int            Edge::layer   ()            const { return mLayer;        }
inline Edge::CostType Edge::cost    ( int layer ) const { return mCost[layer];  }

inline void Edge::setEnabled( bool  enabled ) { mEnabled  = enabled;  }
inline void Edge::setLayer  ( int   layer   ) { mLayer    = layer;    }
inline void Edge::setCost   ( const Edge::CostType &cost , int layer )
{ mCost[layer] = cost; }
// end Edge inline member functions

#endif // EDGE_H
