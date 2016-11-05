#ifndef GRID_H
#define GRID_H

class Grid
{
  public:

    Grid( int tag = 0 , int label = 0 , int capacity = 0 );

    int tag     ();
    int label   ();
    int capacity();

    void setTag     ( int tag      );
    void setLabel   ( int label    );
    void setCapacity( int capacity );

  private:

    int mTag;      // use to distinguish net
    int mLabel;    // use to distinguish step
    int mCapacity;
};

#endif
