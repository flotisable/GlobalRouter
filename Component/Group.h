#ifndef GROUP_H
#define GROUP_H

#include <vector>
#include <ostream>
using std::vector;

#include "Block.h"
#include "Net.h"
#include "Grid.h"
#include "Symmetry.h"
#include "Point.h"

class Group : public Block
{
  public:
  
    inline const vector<double>&  hsplit    ();
    inline const vector<double>&  vsplit    ();
    inline vector<Symmetry>&      symmetrys ();
    inline vector<Block>&         blocks    ();

    vector<vector<Grid>> gridMap();
    vector<Point> connectedPin( Net &net );

    void buildSplit();
    bool netConnected( Net &net );
  
  private:

    int getIndex( const vector<double> &array , double value );

    vector<double>    mHsplit;
    vector<double>    mVsplit;
    vector<Symmetry>  mSymmetrys;
    vector<Block>     mBlocks;
};

std::ostream& operator<<( std::ostream &out , Group &group );

inline const vector<double>&  Group::hsplit   () { return mHsplit;    }
inline const vector<double>&  Group::vsplit   () { return mVsplit;    }
inline vector<Symmetry>&      Group::symmetrys() { return mSymmetrys; }
inline vector<Block>&         Group::blocks   () { return mBlocks;    }

#endif
