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

  GridMap map = group.gridMap();

  for( int i = map.row() - 1 ; i >=0 ; --i )
  {
     for( int j = 0 ; j < map.col() ; ++j )
        switch( map.grid( i , j ).label() )
        {
          case Grid::space:     out << "0"; break;
          case Grid::obstacle:  out << "1"; break;
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


GridMap Group::gridMap( int layer ) const
{
  GridMap map = RoutingRegion::gridMap( layer );

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
           {
              map.grid( i , j ).setLabel( Grid::obstacle );
              map.grid( i , j ).setBlock( &block );
           }
     }
  return map;
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
