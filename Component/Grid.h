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

    inline Grid( int tag = 0 , int label = SPACE , int capacity = 0 );

    inline int tag     () const;
    inline int label   () const;
    inline int capacity() const;

    inline void setTag     ( int tag      );
    inline void setLabel   ( int label    );
    inline void setCapacity( int capacity );

  private:

    int mTag;      // use to distinguish net
    int mLabel;    // use to distinguish step
    int mCapacity;
};

inline Grid::Grid( int tag , int label , int capacity )
  : mTag( tag ) , mLabel( label ) , mCapacity( capacity ) {}

inline int Grid::tag     () const { return mTag;      }
inline int Grid::label   () const { return mLabel;    }
inline int Grid::capacity() const { return mCapacity; }

inline void Grid::setTag     ( int tag      ) { mTag      = tag;      }
inline void Grid::setLabel   ( int label    ) { mLabel    = label;    }
inline void Grid::setCapacity( int capacity ) { mCapacity = capacity; }

#endif
