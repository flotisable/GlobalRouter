#ifndef GRID_H
#define GRID_H

#include <vector>

class Block;
class Edge;

class Grid
{
  public:

    using CostType = double;

    enum Label
    {
      space     = -2,
      obstacle
    };

    enum Direct
    {
      top,
      bottom,
      left,
      right,
      directNum
    };

    inline const Block* block () const;
    inline int          tag   () const;
    inline int          label () const;
    inline CostType     cost  () const;
    inline Edge*        edge  ( Direct direct );
    inline const Edge*  edge  ( Direct direct ) const;

    inline void setBlock( const Block *block );
    inline void setTag  ( int tag       );
    inline void setLabel( int label     );
    inline void setCost ( CostType cost );
    inline void setEdge ( Edge* edge , Direct direct );

  private:

    const Block *mBlock{ nullptr };

    int       mTag  {};         // use to distinguish net
    int       mLabel{ space };  // use to distinguish step
    CostType  mCost {};

    std::vector<Edge*> edges{ directNum , nullptr };
};

inline const Block*   Grid::block () const { return mBlock; }
inline int            Grid::tag   () const { return mTag;   }
inline int            Grid::label () const { return mLabel; }
inline Grid::CostType Grid::cost  () const { return mCost;  }
inline Edge*          Grid::edge  ( Grid::Direct direct )       { return edges[direct]; }
inline const Edge*    Grid::edge  ( Grid::Direct direct ) const { return edges[direct]; }

inline void Grid::setBlock( const Block *block )                { mBlock        = block;  }
inline void Grid::setTag  ( int tag   )                         { mTag          = tag;    }
inline void Grid::setLabel( int label )                         { mLabel        = label;  }
inline void Grid::setCost ( CostType cost )                     { mCost         = cost;   }
inline void Grid::setEdge ( Edge *edge , Grid::Direct direct )  { edges[direct] = edge;   }

#endif
