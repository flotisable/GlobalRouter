#include "Group.h"

#include <iostream>
#include <cassert>
#include <algorithm>
using namespace std;

ostream& operator<<( ostream &out , const Group &group )
{
  out << "[ Group : " << group.name() << " ]\n";

  out << "Horizontal Split : " << group.hsplit().size() << endl;
  for( double point : group.hsplit() ) out << point << endl;
  out << endl;
  
  out << "Vertical Split : " << group.vsplit().size() << endl;
  for( double point : group.vsplit() ) out << point << endl;
  out << endl;
  
  out << "Grids :\n";
  GridMap map = group.gridMap();
  for( int i = map.row() - 1 ; i >=0 ; --i )
  {
     for( const Grid &grid : map.grid( i ) )
        switch( grid.value() )
        {
          case Grid::Value::space:    out << "0"; break;
          case Grid::Value::obstacle: out << "1"; break;
          default:                                break;
        }
     out << endl;
  }
  out << endl;

  out << "Symmetrys : " << group.symmetrys().size() << endl;;
  for( const Symmetry& symmetry : group.symmetrys() ) out << symmetry << endl;
  
  out << "Blocks : " << group.blocks().size() << endl;
  for( const Block& block : group.blocks() ) out << block << endl;

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
      int splitNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

      for( int i = 0 ; i < splitNum ; ++i )
      {
        getline( in , word );
        group.hsplit().push_back( stod( word ) );
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
      int splitNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

      for( int i = 0 ; i < splitNum ; ++i )
      {
        getline( in , word );
        group.vsplit().push_back( stod( word ) );
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
      int SymmetryNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

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
      int blockNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

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

  for( const Symmetry &symmetry : symmetrys() )
     for( const Block &block : symmetry.blocks() )
     {
        int xMin = getIndex( hsplit() , block.left  () );
        int xMax = getIndex( hsplit() , block.right () ) - 1;
        int yMin = getIndex( vsplit() , block.bottom() );
        int yMax = getIndex( vsplit() , block.top   () ) - 1;

        for( int i = yMin ; i <= yMax ; ++i )
           for( int j = xMin ; j <= xMax ; ++j )
           {
              map.grid( i , j ).setValue( Grid::Value::obstacle );
              map.grid( i , j ).setBlock( &block );
           }
     }
  return map;
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

const Block* Group::getBlock( const string &name ) const
{
  for( const Symmetry &symmetry : symmetrys() )
  {
     const Block *block = symmetry.getBlock( name );

     if( block ) return block;
  }
  return RoutingRegion::getBlock( name );
}

void Group::buildSplit()
{
  for( const Symmetry &symmetry : symmetrys() )
     for( const Block &block : symmetry.blocks() )
     {
        hsplit().push_back( block.left   () );
        hsplit().push_back( block.right  () );
        vsplit().push_back( block.top    () );
        vsplit().push_back( block.bottom () );
     }
  RoutingRegion::buildSplit();
}
