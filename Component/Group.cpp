#include "Group.h"

#include <iostream>
#include <cassert>
#include <algorithm>
#include <cstdlib>
using namespace std;

ostream& operator<<( ostream &out , const Group &group )
{
  out << "[ Group : " << group.name() << " ]\n";

  out << "Horizontal Split : " << group.hsplit().size() << endl;
  for( unsigned int i = 0 ; i < group.hsplit().size() ; ++i )
     out << group.hsplit()[i] << endl;
  out << endl;
  
  out << "Vertical Split : " << group.vsplit().size() << endl;
  for( unsigned int i = 0 ; i < group.vsplit().size() ; ++i )
     out << group.vsplit()[i] << endl;
  out << endl;
  
  out << "Grids :\n";
  vector<vector<Grid> > grids = group.gridMap();
  for( int i = static_cast<int>( grids.size() ) - 1 ; i >=0 ; --i )
  {
     for( unsigned j = 0 ; j < grids[i].size() ; ++j )
        switch( grids[i][j].label() )
        {
          case Grid::SPACE:     out << "0"; break;
          case Grid::OBSTACLE:  out << "1"; break;
          default:                          break;
        }
     out << endl;
  }
  out << endl;

  out << "Symmetrys : " << group.symmetrys().size() << endl;;
  for( unsigned int i = 0 ; i < group.symmetrys().size() ; ++i )
     out << group.symmetrys()[i] << endl;
  
  out << "Blocks : " << group.blocks().size() << endl;
  for( unsigned int i = 0 ; i < group.blocks().size() ; ++i )
     out << group.blocks()[i] << endl;

  return out;
}

istream& operator>>( istream &in  , Group &group )
{
  string word;

  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Horizontal Split : " ) != string::npos )
    {
      int splitNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

      for( int i = 0 ; i < splitNum ; ++i )
      {
        getline( in , word );
        group.hsplit().push_back( atof( word.data() ) );
      }
      group.setLeft  ( group.hsplit().front () );
      group.setRight ( group.hsplit().back  () );
      break;
    }
  }

  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Vertical Split : " ) != string::npos )
    {
      int splitNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

      for( int i = 0 ; i < splitNum ; ++i )
      {
        getline( in , word );
        group.vsplit().push_back( atof( word.data() ) );
      }
      group.setBottom( group.vsplit().front () );
      group.setTop   ( group.vsplit().back  () );
      break;
    }
  }

  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Symmetrys : " ) != string::npos )
    {
      int SymmetryNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

      for( int i = 0 ; !in.eof() && i < SymmetryNum ; )
      {
        getline( in , word );

        if( word.find( "[ Symmetry : " ) != string::npos )
        {
          Symmetry      symmetry;
          unsigned int  start = word.find( ": " ) + 2;
          unsigned int  end   = word.rfind( ' ' );

          symmetry.setName( word.substr( start , end - start ) );

          in >> symmetry;

          group.symmetrys().push_back( symmetry );
          ++i;
        }
      }
      break;
    }
  }

  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Blocks : " ) != string::npos )
    {
      int blockNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

      for( int i = 0 ; i < blockNum ; ++i )
      {
        Block block;

        in >> block;
        group.blocks().push_back( block );
      }
      break;
    }
  }

  return in;
}


vector<vector<Grid> > Group::gridMap() const
{
  assert( mVsplit.size() > 0 && mHsplit.size() > 0 );
  vector<vector<Grid> > grids( mVsplit.size() - 1 , vector<Grid>( mHsplit.size() - 1 ) );

  for( unsigned int i = 0 ; i < symmetrys().size() ; ++i )
     for( unsigned int j = 0 ; j < symmetrys()[i].blocks().size() ; ++j )
     {
        const Block &block = symmetrys()[i].blocks()[j];

        int xMin = getIndex( mHsplit , block.left  () );
        int xMax = getIndex( mHsplit , block.right () ) - 1;
        int yMin = getIndex( mVsplit , block.bottom() );
        int yMax = getIndex( mVsplit , block.top   () ) - 1;

        for( int i = yMin ; i <= yMax ; ++i )
           for( int j = xMin ; j <= xMax ; ++j )
              grids[i][j].setLabel( Grid::OBSTACLE );
     }

  for( unsigned int i = 0 ; i < blocks().size() ; ++i )
  {
     int xMin = getIndex( mHsplit , blocks()[i].left  () );
     int xMax = getIndex( mHsplit , blocks()[i].right () ) - 1;
     int yMin = getIndex( mVsplit , blocks()[i].bottom() );
     int yMax = getIndex( mVsplit , blocks()[i].top   () ) - 1;

     for( int i = yMin ; i <= yMax ; ++i )
        for( int j = xMin ; j <= xMax ; ++j )
           grids[i][j].setLabel( Grid::OBSTACLE );
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
  for( unsigned int i = 0 ; i < symmetrys().size() ; ++i )
  {
    Block *block = symmetrys()[i].getBlock( name );
    
    if( block ) return block;
  }
  return RoutingRegion::getBlock( name );
}

const Block* Group::getBlock( const string &name ) const
{
  for( unsigned int i = 0 ; i < symmetrys().size() ; ++i )
  {
     const Block *block = symmetrys()[i].getBlock( name );

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

  for( unsigned int i = 0 ; i < symmetrys().size() ; ++i )
     for( unsigned int j = 0 ; j < symmetrys()[i].blocks().size() ; ++j )
     {
        const Block &block = symmetrys()[i].blocks()[j];

        mHsplit.push_back( block.left   () );
        mHsplit.push_back( block.right  () );
        mVsplit.push_back( block.top    () );
        mVsplit.push_back( block.bottom () );
     }

  for( unsigned int i = 0 ; i < blocks().size() ; ++i )
  {
     mHsplit.push_back( blocks()[i].left   () );
     mHsplit.push_back( blocks()[i].right  () );
     mVsplit.push_back( blocks()[i].top    () );
     mVsplit.push_back( blocks()[i].bottom () );
  }

  sort( mHsplit.begin() , mHsplit.end() );
  sort( mVsplit.begin() , mVsplit.end() );

  vector<double>::iterator it = unique( mHsplit.begin() , mHsplit.end() );

  mHsplit.resize( distance( mHsplit.begin() , it ) );

  it = unique( mVsplit.begin() , mVsplit.end() );

  mVsplit.resize( distance( mVsplit.begin() , it ) );
}
