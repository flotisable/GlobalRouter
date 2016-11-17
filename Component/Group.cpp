#include "Group.h"

#include <iostream>
#include <cassert>
#include <algorithm>
using namespace std;

ostream& operator<<( ostream &out , Group &group )
{
  out << "[ Group : " << group.name() << " ]\n";

  group.buildSplit();

  out << "Horizontal Split : " << group.hsplit().size() << endl;
  for( double point : group.hsplit() ) out << point << endl;
  out << endl;
  
  out << "Vertical Split : " << group.vsplit().size() << endl;
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

  out << "Symmetrys : " << group.symmetrys().size() << endl;;
  for( Symmetry& symmetry : group.symmetrys() ) out << symmetry << endl;
  
  out << "Blocks : " << group.blocks().size() << endl;
  for( const Block& block : group.blocks() ) out << block << endl;

  return out;
}

vector<vector<Grid>> Group::gridMap()
{
  assert( mVsplit.size() > 0 && mHsplit.size() > 0 );
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

  double maxH = 0;
  double maxV = 0;

  for( unsigned int i = 0 ; i < mHsplit.size() - 1 ; ++i )
     if( mHsplit[i+1] - mHsplit[i] > maxH ) maxH = mHsplit[i+1] - mHsplit[i];

  for( unsigned int i = 0 ; i < mVsplit.size() - 1 ; ++i )
     if( mVsplit[i+1] - mVsplit[i] > maxV ) maxV = mVsplit[i+1] - mVsplit[i];

  for( unsigned int i = 0 ; i < grids.size() ; ++i )
     for( unsigned int j = 0 ; j < grids[0].size() ; ++j )
     {
        grids[i][j].setCostX( maxH - ( mHsplit[j+1] - mHsplit[j] ) );
        grids[i][j].setCostY( maxV - ( mVsplit[i+1] - mVsplit[i] ) );
     }

  return grids;
}

Block* Group::getBlock( const string &name )
{
  for( Symmetry &symmetry : symmetrys() )
  {
     Block *block = symmetry.getBlock( name );

     if( block ) return block;
  }

  return RoutingRegion::getBlock( name );
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
