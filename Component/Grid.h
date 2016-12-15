#ifndef GRID_H
#define GRID_H

#include <vector>

class RoutingRegion;

class Grid
{
  public:

    using CostType = double;

    enum Label
    {
      SPACE     = -2,
      OBSTACLE
    };

    inline explicit Grid( int layer = 1 );

    inline const RoutingRegion* region    () const;
    inline int                  tag       () const;
    inline int                  label     () const;
    inline CostType             cost      () const;
    inline CostType             costTop   ( int layer = 0 ) const;
    inline CostType             costBottom( int layer = 0 ) const;
    inline CostType             costLeft  ( int layer = 0 ) const;
    inline CostType             costRight ( int layer = 0 ) const;

    inline void setRegion     ( const RoutingRegion *region );
    inline void setTag        ( int tag       );
    inline void setLabel      ( int label     );
    inline void setCost       ( CostType cost );
    inline void setCostTop    ( CostType cost , int layer = 0 );
    inline void setCostBottom ( CostType cost , int layer = 0 );
    inline void setCostLeft   ( CostType cost , int layer = 0 );
    inline void setCostRight  ( CostType cost , int layer = 0 );

  private:

    struct BoundaryCost
    {
      CostType top     = 0;
      CostType bottom  = 0;
      CostType left    = 0;
      CostType right   = 0;
    };

    const RoutingRegion *mRegion = nullptr;

    int       mTag    = 0;      // use to distinguish net
    int       mLabel  = SPACE;  // use to distinguish step
    CostType  mCost   = 0;

    std::vector<BoundaryCost> boundaryCost;
};

inline Grid::Grid( int layer ) { boundaryCost.resize( layer ); }

inline const RoutingRegion* Grid::region    () const { return mRegion;      }
inline int                  Grid::tag       () const { return mTag;         }
inline int                  Grid::label     () const { return mLabel;       }
inline Grid::CostType       Grid::cost      () const { return mCost;        }
inline Grid::CostType       Grid::costTop   ( int layer ) const
{ return boundaryCost[layer].top; }
inline Grid::CostType       Grid::costBottom( int layer ) const
{ return boundaryCost[layer].bottom;  }
inline Grid::CostType       Grid::costLeft  ( int layer ) const
{ return boundaryCost[layer].left;    }
inline Grid::CostType       Grid::costRight ( int layer ) const
{ return boundaryCost[layer].right;   }

inline void Grid::setRegion     ( const RoutingRegion *region ) { mRegion     = region; }
inline void Grid::setTag        ( int tag       )               { mTag        = tag;    }
inline void Grid::setLabel      ( int label     )               { mLabel      = label;  }
inline void Grid::setCost       ( CostType cost )               { mCost       = cost;   }
inline void Grid::setCostTop    ( CostType cost , int layer )
{ boundaryCost[layer].top = cost; }
inline void Grid::setCostBottom ( CostType cost , int layer )
{ boundaryCost[layer].bottom = cost; }
inline void Grid::setCostLeft   ( CostType cost , int layer )
{ boundaryCost[layer].left = cost; }
inline void Grid::setCostRight  ( CostType cost , int layer )
{ boundaryCost[layer].right = cost; }

#endif
