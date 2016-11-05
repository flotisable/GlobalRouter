#include "Router.h"

#include <fstream>
#include <string>
#include <cctype>
#include <algorithm>
using namespace std;

#include "Component/Block.h"

bool Router::readBlock( const string &fileName )
{
  constexpr double unit = 0.01; // 0.01u

  ifstream  file( fileName );
  Block     block;

  while( file.get() != '\n' ); // 忽略第一行 ignore first line
  
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
      double lbX;
      double lbY;
      double height;
      double width;
      string name;
    
      file >> lbX >> lbY >> width >> height >> name;
      
      // 捨去 Group  discard group
      if( name == "G" )
      {
        while( file.get() != '\n' );
        continue;
      }
      
      /*
        因為 Block 設計的關係，要先設定 Height , Width 再設定 LeftButtom 才不會出問題
        because of the design of Block, we need to setup Hieght and Width first, and then
        setup LeftButtom
      */
      block.setName       ( name );
      block.setHeight     ( height * unit );
      block.setWidth      ( width );
      block.setLeftButtom ( lbX * unit , lbY *unit );
      
      blocks.push_back( block );
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
  
  file << "Blocks : ";
  for( const Block &block : blocks ) file << block << endl;
  file << endl;
  
  file << "Nets : ";
  for( const Net &net : nets );
}
