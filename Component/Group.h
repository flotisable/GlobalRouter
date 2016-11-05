#ifndef GROUP_H
#define GROUP_H

#include <vector>
#include <ostream>
using std::vector;

#include "Block.h"
#include "Net.h"
#include "Grid.h"
#include "Symmetry.h"

class Group : public Block
{
  public:
  
    inline vector<Symmetry>&  symmetrys ();
    inline vector<Block>&     blocks    ();
  
    vector<vector<Grid>> gridMap();
  
  private:
  
    vector<double>    mHsplit;
    vector<double>    mVsplit;
    vector<Symmetry>  mSymmetrys;
    vector<Block>     mBlocks;
};

std::ostream& operator<<( std::ostream &out , Group &group );

inline vector<Symmetry>&  Group::symmetrys() { return mSymmetrys; }
inline vector<Block>&     Group::blocks   () { return mBlocks;    }

#endif
