#ifndef GRID_H
#define GRID_H

#include <vector>

class Block;

class Grid
{
  public:

    typedef double CostType;

    enum Label
    {
      SPACE     = -2,
      OBSTACLE
    };

    inline explicit Grid( int layer = 1 );

    inline const Block* block     () const;
    inline int          tag       () const;
    inline int          label     () const;
    inline int          layer     () const;
    inline CostType     cost      () const;
    inline CostType     costTop   ( int layer = 0 ) const;
    inline CostType     costBottom( int layer = 0 ) const;
    inline CostType     costLeft  ( int layer = 0 ) const;
    inline CostType     costRight ( int layer = 0 ) const;

    inline void setBlock      ( const Block *block );
    inline void setTag        ( int tag       );
    inline void setLabel      ( int label     );
    inline void setLayer      ( int layer     );
    inline void setCost       ( CostType cost );
    inline void setCostTop    ( CostType cost , int layer = 0 );
    inline void setCostBottom ( CostType cost , int layer = 0 );
    inline void setCostLeft   ( CostType cost , int layer = 0 );
    inline void setCostRight  ( CostType cost , int layer = 0 );

  private:

    struct BoundaryCost
    {
      CostType top;
      CostType bottom;
      CostType left;
      CostType right;

      inline explicit BoundaryCost();
    };

    const Block *mBlock;

    int       mTag;   // use to distinguish net
    int       mLabel; // use to distinguish step
    int       mLayer;
    CostType  mCost;

    std::vector<BoundaryCost> boundaryCost;
};

inline Grid::Grid( int layer ) :  mBlock( NULL ) , mTag( 0 ) , mLabel( Grid::SPACE ) ,
                                  mLayer( 0 ) , mCost( 0 )
{ boundaryCost.resize( layer ); }

inline const Block*   Grid::block     () const { return mBlock;  }
inline int            Grid::tag       () const { return mTag;     }
inline int            Grid::label     () const { return mLabel;   }
inline int            Grid::layer     () const { return mLayer;   }
inline Grid::CostType Grid::cost      () const { return mCost;    }
inline Grid::CostType Grid::costTop   ( int layer ) const
{ return boundaryCost[layer].top; }
inline Grid::CostType Grid::costBottom( int layer ) const
{ return boundaryCost[layer].bottom;  }
inline Grid::CostType Grid::costLeft  ( int layer ) const
{ return boundaryCost[layer].left;    }
inline Grid::CostType Grid::costRight ( int layer ) const
{ return boundaryCost[layer].right;   }

inline void Grid::setBlock      ( const Block *block )  { mBlock  = block;  }
inline void Grid::setTag        ( int tag   )           { mTag    = tag;    }
inline void Grid::setLabel      ( int label )           { mLabel  = label;  }
inline void Grid::setLayer      ( int layer )           { mLayer  = layer;  }
inline void Grid::setCost       ( CostType cost )       { mCost   = cost;   }
inline void Grid::setCostTop    ( CostType cost , int layer )
{ boundaryCost[layer].top = cost; }
inline void Grid::setCostBottom ( CostType cost , int layer )
{ boundaryCost[layer].bottom = cost; }
inline void Grid::setCostLeft   ( CostType cost , int layer )
{ boundaryCost[layer].left = cost; }
inline void Grid::setCostRight  ( CostType cost , int layer )
{ boundaryCost[layer].right = cost; }


Grid::BoundaryCost::BoundaryCost() : top( 0 ) , bottom( 0 ) , left( 0 ) , right( 0 ) {}

#endif
