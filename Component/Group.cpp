#include "Group.h"

#include <iostream>
#include <algorithm>

// Group non-member functions
std::ostream& operator<<( std::ostream &out , const Group &group )
{
  using std::endl;

  out << "[ Group : " << group.name() << " ]\n";

  out << "Horizontal Split : " << group.hsplit().size() << endl;
  for( double point : group.hsplit() ) out << point << endl;
  out << endl;

  out << "Vertical Split : " << group.vsplit().size() << endl;
  for( double point : group.vsplit() ) out << point << endl;
  out << endl;

  out << "Grids :\n";
  outputGridMapValue( out , group.gridMap() );

  out << "Symmetrys : " << group.symmetrys().size() << endl;;
  for( const Symmetry& symmetry : group.symmetrys() ) out << symmetry << endl;

  out << "Blocks : " << group.blocks().size() << endl;
  for( const Block& block : group.blocks() ) out << block << endl;

  return out;
}

std::istream& operator>>( std::istream &in  , Group &group )
{
  string word;

  // read horizontal split
  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Horizontal Split : " ) != string::npos )
    {
      const int splitNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

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
  // end read horizontal split

  // read vertical split
  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Vertical Split : " ) != string::npos )
    {
      const int splitNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

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
  // end read vertical split

  // read symmetrys
  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Symmetrys : " ) != string::npos )
    {
      const int SymmetryNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

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
  // end read symmetrys

  // read blocks
  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Blocks : " ) != string::npos )
    {
      const int blockNum = stoi( word.substr( word.rfind( ' ' ) + 1 ) );

      for( int i = 0 ; i < blockNum ; ++i )
      {
        Block block;

        in >> block;
        group.blocks().push_back( block );
      }
      break;
    }
  }
  // end read blocks

  return in;
}
// end Group non-member functions

// Group member functions
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
    if( Block *block = symmetry.getBlock( name ) ) return block;
  return RoutingRegion::getBlock( name );
}

const Block* Group::getBlock( const string &name ) const
{
  return const_cast<Group*>( this )->getBlock( name );
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
// end Group member functions