#ifndef GRID_H
#define GRID_H

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

    inline const RoutingRegion* region    () const;
    inline int                  tag       () const;
    inline int                  label     () const;
    inline CostType             cost      () const;
    inline CostType             costTop   () const;
    inline CostType             costBottom() const;
    inline CostType             costLeft  () const;
    inline CostType             costRight () const;

    inline void setRegion     ( const RoutingRegion *region );
    inline void setTag        ( int tag       );
    inline void setLabel      ( int label     );
    inline void setCost       ( CostType cost );
    inline void setCostTop    ( CostType cost );
    inline void setCostBottom ( CostType cost );
    inline void setCostLeft   ( CostType cost );
    inline void setCostRight  ( CostType cost );

  private:

    const RoutingRegion *mRegion = nullptr;

    int mTag    = 0;      // use to distinguish net
    int mLabel  = SPACE;  // use to distinguish step

    CostType mCost        = 0;
    CostType mCostTop     = 0;
    CostType mCostBottom  = 0;
    CostType mCostLeft    = 0;
    CostType mCostRight   = 0;
};

inline const RoutingRegion* Grid::region    () const { return mRegion;      }
inline int                  Grid::tag       () const { return mTag;         }
inline int                  Grid::label     () const { return mLabel;       }
inline Grid::CostType       Grid::cost      () const { return mCost;        }
inline Grid::CostType       Grid::costTop   () const { return mCostTop;     }
inline Grid::CostType       Grid::costBottom() const { return mCostBottom;  }
inline Grid::CostType       Grid::costLeft  () const { return mCostLeft;    }
inline Grid::CostType       Grid::costRight () const { return mCostRight;   }

inline void Grid::setRegion     ( const RoutingRegion *region ) { mRegion     = region; }
inline void Grid::setTag        ( int tag     )                 { mTag        = tag;    }
inline void Grid::setLabel      ( int label   )                 { mLabel      = label;  }
inline void Grid::setCost       ( CostType cost )               { mCost       = cost;   }
inline void Grid::setCostTop    ( CostType cost )               { mCostTop    = cost;   }
inline void Grid::setCostBottom ( CostType cost )               { mCostBottom = cost;   }
inline void Grid::setCostLeft   ( CostType cost )               { mCostLeft   = cost;   }
inline void Grid::setCostRight  ( CostType cost )               { mCostRight  = cost;   }

#endif
