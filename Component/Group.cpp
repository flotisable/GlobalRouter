#include "Group.h"

#include <iostream>
#include <cstdlib>

// Group non-member functions
std::ostream& operator<<( std::ostream &out , const Group &group )
{
  using std::endl;

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
  outputGridMapValue( out , group.gridMap() );

  out << "Symmetrys : " << group.symmetrys().size() << endl;;
  for( unsigned int i = 0 ; i < group.symmetrys().size() ; ++i )
     out << group.symmetrys()[i] << endl;

  out << "Blocks : " << group.blocks().size() << endl;
  for( unsigned int i = 0 ; i < group.blocks().size() ; ++i )
     out << group.blocks()[i] << endl;

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
      const int splitNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

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
  // end read horizontal split

  // read vertical split
  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Vertical Split : " ) != string::npos )
    {
      const int splitNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

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
  // end read vertical split

  // read symmetrys
  while( !in.eof() )
  {
    getline( in , word );

    if( word.find( "Symmetrys : " ) != string::npos )
    {
      const int SymmetryNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

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
      const int blockNum = atoi( word.substr( word.rfind( ' ' ) + 1 ).data() );

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

  for( unsigned int i = 0 ; i < symmetrys().size() ; ++i )
     for( unsigned int j = 0 ; j < symmetrys()[i].blocks().size() ; ++j )
     {
        const Block &block = symmetrys()[i].blocks()[j];

        int xMin = getIndex( hsplit() , block.left  () );
        int xMax = getIndex( hsplit() , block.right () ) - 1;
        int yMin = getIndex( vsplit() , block.bottom() );
        int yMax = getIndex( vsplit() , block.top   () ) - 1;

        for( int i = yMin ; i <= yMax ; ++i )
           for( int j = xMin ; j <= xMax ; ++j )
           {
              map.grid( i , j ).setValue( Grid::obstacle );
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
  return const_cast<Group*>( this )->getBlock( name );
}

void Group::buildSplit()
{
  for( unsigned int i = 0 ; i < symmetrys().size() ; ++i )
     for( unsigned int j = 0 ; j < symmetrys()[i].blocks().size() ; ++j )
     {
        const Block &block = symmetrys()[i].blocks()[j];

        hsplit().push_back( block.left   () );
        hsplit().push_back( block.right  () );
        vsplit().push_back( block.top    () );
        vsplit().push_back( block.bottom () );
     }
  RoutingRegion::buildSplit();
}
// end Group member functions