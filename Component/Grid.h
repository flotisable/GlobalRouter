#ifndef GRID_H
#define GRID_H

class Grid
{
  public:

    enum Label
    {
      SPACE     = -1,
      OBSTACLE
    };

    inline Grid( int tag = 0 , int label = SPACE , double cost = 0.0 );

    inline int    tag   () const;
    inline int    label () const;
    inline double cost  () const;
    inline double costX () const;
    inline double costY () const;

    inline void setTag  ( int tag     );
    inline void setLabel( int label   );
    inline void setCost ( double cost );
    inline void setCostX( double cost );
    inline void setCostY( double cost );

  private:

    int mTag;      // use to distinguish net
    int mLabel;    // use to distinguish step

    double mCost;
    double mCostX;
    double mCostY;
};

inline Grid::Grid( int tag , int label , double cost )
  : mTag( tag ) , mLabel( label ) , mCost( cost ) {}

inline int    Grid::tag   () const { return mTag;   }
inline int    Grid::label () const { return mLabel; }
inline double Grid::cost  () const { return mCost;  }
inline double Grid::costX () const { return mCostX; }
inline double Grid::costY () const { return mCostY; }

inline void Grid::setTag  ( int tag     ) { mTag    = tag;    }
inline void Grid::setLabel( int label   ) { mLabel  = label;  }
inline void Grid::setCost ( double cost ) { mCost   = cost;   }
inline void Grid::setCostX( double cost ) { mCostX  = cost;   }
inline void Grid::setCostY( double cost ) { mCostY  = cost;   }

#endif
