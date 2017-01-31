#ifndef GRID_H
#define GRID_H

#include <vector>

class Block;
class Edge;

class Grid
{
  public:

    using CostType = double;

    enum class Value
    {
      space,
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
    inline int          layer () const;
    inline Value        value () const;
    inline CostType     cost  () const;
    inline Edge*        edge  ( Direct direct );
    inline const Edge*  edge  ( Direct direct ) const;

    inline void setBlock( const Block *block  );
    inline void setTag  ( int         tag     );
    inline void setLabel( int         label   );
    inline void setLayer( int         layer   );
    inline void setValue( Value       value   );
    inline void setCost ( CostType    cost    );
    inline void setEdge ( Edge* edge , Direct direct );

  private:

    const Block *mBlock {};
    int         mTag    {};               // use to distinguish net
    int         mLabel  {};               // use to distinguish step
    int         mLayer  {};               // save the last connect layer with minimum cost
    Value       mValue  { Value::space }; // use to distinguish obstacle
    CostType    mCost   {};

    std::vector<Edge*> edges{ directNum , nullptr };
};

inline const Block*   Grid::block () const { return mBlock; }
inline int            Grid::tag   () const { return mTag;   }
inline int            Grid::label () const { return mLabel; }
inline int            Grid::layer () const { return mLayer; }
inline Grid::Value    Grid::value () const { return mValue; }
inline Grid::CostType Grid::cost  () const { return mCost;  }
inline Edge*          Grid::edge  ( Grid::Direct direct )       { return edges[direct]; }
inline const Edge*    Grid::edge  ( Grid::Direct direct ) const { return edges[direct]; }

inline void Grid::setBlock( const Block *block  )               { mBlock        = block;  }
inline void Grid::setTag  ( int         tag     )               { mTag          = tag;    }
inline void Grid::setLabel( int         label   )               { mLabel        = label;  }
inline void Grid::setLayer( int         layer   )               { mLayer        = layer;  }
inline void Grid::setValue( Value       value   )               { mValue        = value;  }
inline void Grid::setCost ( CostType    cost    )               { mCost         = cost;   }
inline void Grid::setEdge ( Edge *edge , Grid::Direct direct )  { edges[direct] = edge;   }

#endif
