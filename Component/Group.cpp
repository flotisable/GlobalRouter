#include "Group.h"

#include <algorithm>
using namespace std;

vector<vector<Grid>> Group::gridMap()
{
  vector<vector<Grid>> grids( mVsplit.size() - 1 , vector<Grid>( mHsplit.size() - 1 ) );

  for( Symmetry &symmetry : symmetrys() )
     for( const Block &block : symmetry.blocks() )
     {
        int x = getIndex( mHsplit , block.left  () );
        int y = getIndex( mVsplit , block.bottom() );
        
        grids[y][x].setLabel( Grid::OBSTACLE );
     }

  for( const Block &block : blocks() )
  {
     int x = getIndex( mHsplit , block.left  () );
     int y = getIndex( mVsplit , block.bottom() );

     grids[y][x].setLabel( Grid::OBSTACLE );
  }
  
  return grids;
}

void Group::buildSplit()
{
  mHsplit.push_back( left   () );
  mHsplit.push_back( right  () );
  mVsplit.push_back( top    () );
  mVsplit.push_back( bottom () );

  for( Symmetry &symmetry : symmetrys() )
     for( const Block &block : symmetry.blocks() )
     {
        mHsplit.push_back( block.left   () );
        mHsplit.push_back( block.right  () );
        mVsplit.push_back( block.top    () );
        mVsplit.push_back( block.bottom () );
     }

  for( const Block &block : blocks() )
  {
     mHsplit.push_back( block.left   () );
     mHsplit.push_back( block.right  () );
     mVsplit.push_back( block.top    () );
     mVsplit.push_back( block.bottom () );
  }

  sort( mHsplit.begin() , mHsplit.end() );
  sort( mVsplit.begin() , mVsplit.end() );

  auto it = unique( mHsplit.begin() , mHsplit.end() );
  
  mHsplit.resize( distance( mHsplit.begin() , it ) );
  mHsplit.shrink_to_fit();
  
  it = unique( mVsplit.begin() , mVsplit.end() );
  
  mVsplit.resize( distance( mVsplit.begin() , it ) );
  mVsplit.shrink_to_fit();
}


int Group::getIndex( const vector<double> &array , double value )
{
  for( int i = 0 ; i < static_cast<int>( array.size() ) ; ++i )
     if( array[i] == value ) return i;
  return -1;
}


ostream& operator<<( ostream &out , Group &group )
{
  out << "[ Group : " << group.name() << "]\n";

  group.buildSplit();

  out << "Horizontal Split :\n";
  for( double point : group.hsplit() ) out << point << endl;
  out << endl;
  
  out << "Vertical Split :\n";
  for( double point : group.vsplit() ) out << point << endl;
  out << endl;
  
  out << "Grids :\n";
  vector<vector<Grid>> grids = group.gridMap();
  for( int i = static_cast<int>( grids.size() ) - 1 ; i >=0 ; --i )
  {
     for( const Grid &grid : grids[i] )
        switch( grid.label() )
        {
          case Grid::SPACE:     out << "0"; break;
          case Grid::OBSTACLE:  out << "1"; break;
          default:                          break;
        }
     out << endl;
  }
  out << endl;

  out << "Symmetrys :\n";
  for( Symmetry& symmetry : group.symmetrys() ) out << symmetry << endl;
  
  out << "Blocks :\n";
  for( const Block& block : group.blocks() ) out << block << endl;

  return out;
}
