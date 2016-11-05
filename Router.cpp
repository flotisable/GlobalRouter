#include "Router.h"

#include <fstream>
#include <string>
#include <cctype>
#include <algorithm>
using namespace std;

#include "Component/Block.h"

bool Router::readBlock( const string &fileName , const string &groupFileName )
{
  constexpr double unit = 0.01; // 0.01u

  ifstream  file( fileName );
  int       groupIndex = 0;

  readGroup( groupFileName );

  while( !file.eof() )
  {
    // 判斷註解 test if it is comment
    if( file.peek() == '/' )
    {
      char c = file.get();

      if( file.peek() == '/' )
      {
        while( file.get() != '\n' );
        continue;
      }
      else                      file.putback( c );
    }
    // end 判斷註解 test if it is comment
    
    if( isdigit( file.peek() ) )
    {
      Block  block;
      double lbX;
      double lbY;
      double height;
      double width;
      string name;
    
      file >> lbX >> lbY >> width >> height >> name;
      
      /*
        因為 Block 設計的關係，要先設定 Height , Width 再設定 LeftButtom 才不會出問題
        because of the design of Block, we need to setup Hieght and Width first, and then
        setup LeftButtom
      */
      
      if( name == "ALL" )
      {
        setHeight     ( height * unit );
        setWidth      ( width * unit );
        setLeftBottom ( lbX * unit , lbY *unit );
        while( file.get() != '\n' );
        continue;
      }
      
      block.setName       ( name );
      block.setHeight     ( height * unit );
      block.setWidth      ( width * unit );
      block.setLeftBottom ( lbX * unit , lbY *unit );

      if( name == "G" ) // 設定 Group set group
      {
        groups[groupIndex].setHeight    ( height * unit );
        groups[groupIndex].setWidth     ( width * unit );
        groups[groupIndex].setLeftBottom( lbX * unit , lbY * unit );
        
        groupIndex++;
      }
      else // 偵測 Block 是否屬於 Group  find if block is contained in a group
      {
        for( Group &group : groups )
        {
           for( Symmetry &symmetry : group.symmetrys() )
           {
              auto it = find_if(  symmetry.blocks().begin() , symmetry.blocks().end() ,
                                  [&]( const Block &b )
                                  { return b.name() == block.name(); } );

              if( it != symmetry.blocks().end() )
              {
                *it = block;
                goto match;
              }
           }
           auto it = find_if( group.blocks().begin() , group.blocks().end() ,
                              [&]( const Block &b ) { return b.name() == block.name(); } );

           if( it != group.blocks().end() )
           {
             *it = block;
             goto match;
           }
        }
        goto noMatch;
        match:    continue;
        noMatch:  blocks.push_back( block );
      }

      mHsplit.push_back( block.left()   );
      mHsplit.push_back( block.right()  );
      mVsplit.push_back( block.top()    );
      mVsplit.push_back( block.bottom() );
    }
    
    while( file.get() != '\n' )
      if( file.eof() ) break;
  }
  
  sort( mHsplit.begin() , mHsplit.end() );
  sort( mVsplit.begin() , mVsplit.end() );
  
  auto it = unique( mHsplit.begin() , mHsplit.end() );
  
  mHsplit.resize( distance( mHsplit.begin() , it ) );
  mHsplit.shrink_to_fit();
  
  it = unique( mVsplit.begin() , mVsplit.end() );
  
  mVsplit.resize( distance( mVsplit.begin() , it ) );
  mVsplit.shrink_to_fit();
  
  return true;
}

bool Router::readNets( const string &fileName )
{
  return true;
}

bool Router::route()
{
  return true;
}

void Router::outputData( const string &fileName )
{
  ofstream file( fileName );
  
  file << "[ Graph ]\n";
  file << "Horizontal Split : \n";
  
  for( double point : mHsplit ) file << point << endl;
  file << endl;
  
  file << "Vertical Split : \n";
  
  for( double point : mVsplit ) file << point << endl;
  file << endl;
  
  file << "[ Groups ]\n";
  for( Group &group : groups ) file << group << endl;
  
  file << "[ Blocks ]\n";
  for( const Block &block : blocks ) file << block << endl;
  file << endl;
  
  file << "[ Nets ]\n";
  for( const Net &net : nets );
  file << endl;
}


bool Router::readGroup( const string &fileName )
{
  ifstream          file( fileName );
  string            word;
  vector<Symmetry>  symmetrys;

  while( !file.eof() )
  {
    file >> word;
    
    if      ( word == "Symmetry" )
    {
      Symmetry symmetry;

      file >> word;
      
      if( word[0] != 'S' )  continue;
      else                  symmetry.setName( word );
      file >> word; // pass one word

      file >> word; symmetry.blocks().push_back( Block( word ) );
      file >> word; symmetry.blocks().push_back( Block( word ) );
      
      symmetrys.push_back( symmetry );
    }
    else if ( word == "Group" )
    {
      Group group;
      int   blockNum;
      
      file >> word;
      
      if( word[0] != 'G' )  continue;
      else                  group.setName( word );
      
      file >> blockNum >> word; // pass one word
      
      for( int i = 0 ; i < blockNum ; ++i )
      {
         file >> word;
         
         if( word[0] == 'S' )
         {
           int index = stoi( word.substr( 1 ) ) - 1;
           
           group.symmetrys().push_back( symmetrys[index] );
         }
         else
           group.blocks().push_back( Block( word ) );
      }
      groups.push_back( group );
    }
  }

  return true;
}
