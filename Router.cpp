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
      
      if( name == "ALL" ) // 設定邊界 setup boudary
      {
        setHeight     ( height * unit );
        setWidth      ( width * unit );
        setLeftBottom ( lbX * unit , lbY *unit );
        mHsplit.push_back( left()   );
        mHsplit.push_back( right()  );
        mVsplit.push_back( top()    );
        mVsplit.push_back( bottom() );
        while( file.get() != '\n' );
        continue;
      }
      
      block.setName       ( name );
      block.setHeight     ( height * unit );
      block.setWidth      ( width * unit );
      block.setLeftBottom ( lbX * unit , lbY *unit );

      if( name[0] == 'G' ) // 設定 Group set group
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
              Block* const blockPtr = getBlock( symmetry.blocks() , block.name() );

              if( blockPtr )
              {
                *blockPtr = block;
                goto match;
              }
           }
           Block* const blockPtr = getBlock( group.blocks() , block.name() );

           if( blockPtr )
           {
             *blockPtr = block;
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

  // 排序並刪去重複項 sort and remove repeat points
  sort( mHsplit.begin() , mHsplit.end() );
  sort( mVsplit.begin() , mVsplit.end() );
  
  auto it = unique( mHsplit.begin() , mHsplit.end() );
  
  mHsplit.resize( distance( mHsplit.begin() , it ) );
  mHsplit.shrink_to_fit();
  
  it = unique( mVsplit.begin() , mVsplit.end() );
  
  mVsplit.resize( distance( mVsplit.begin() , it ) );
  mVsplit.shrink_to_fit();
  // end 排序並刪去重複項 sort and remove repeat points
  
  return true;
}

bool Router::readNets( const string &fileName )
{
  constexpr double unit = 0.01;

  ifstream  file( fileName );
  string    word;
  
  while( !file.eof() )
  {
    file >> word;
    
    if( word == "NetDegree" )
    {
      Net net;
      int currentDensity;
      int pinNum;
      
      file >> word >> pinNum >> word >> currentDensity;
      net.setName           ( word );
      net.setCurrentDensity ( currentDensity );
      
      for( int i = 0 ; i < pinNum ; i++ )
      {
         Pin    pin;
         string blockName;
         double x;
         double y;

         file >> blockName >> word >> word >> x >> y;
         
         pin.set( x * unit + width() / 2 , y * unit + height() / 2 );
         
         for( Group &group : groups )
         {
            for( Symmetry &symmetry : group.symmetrys() )
            {
               Block* const block = getBlock( symmetry.blocks() , blockName );

               if( block )
               {
                 pin.setConnect( block );
                 goto match;
               }
            }
            
            Block* const block = getBlock( group.blocks() , blockName );

            if( block )
            {
              pin.setConnect( block );
              goto match;
            }
         }
         match:

         Block* const block = getBlock( blocks , blockName );

         if( block ) pin.setConnect( block );
         
         net.pins().push_back( pin );
      }
      
      nets.push_back( net );
    }
  }

  return true;
}

bool Router::route()
{
  for( Group &group : groups )
  {
     group.buildSplit();
     mRouter->setGrids( group.gridMap() );
     
     for( Net &net : nets )
     {
        if( !group.netConnected( net ) ) continue;

        mRouter->setPins( group.connectedPin( net ) );
        mRouter->route();
        mRouter->saveNet( net );
     }
  }

  mRouter->setGrids( gridMap() );

  for( Net &net : nets )
  {
     mRouter->setPins( connectedPin( net ) );
     mRouter->route();
     mRouter->saveNet( net );
  }

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
  
  file << "Grids :\n";
  vector<vector<Grid>> grids = gridMap();
  for( int i = static_cast<int>( grids.size() ) - 1 ; i >=0 ; --i )
  {
     for( const Grid &grid : grids[i] )
        switch( grid.label() )
        {
          case Grid::SPACE:     file << "0";  break;
          case Grid::OBSTACLE:  file << "1";  break;
          default:                            break;
        }
     file << endl;
  }
  file << endl;
  
  file << "[ Groups ]\n";
  for( Group &group : groups ) file << group << endl;
  
  file << "[ Blocks ]\n";
  for( const Block &block : blocks ) file << block << endl;
  file << endl;
  
  file << "[ Nets ]\n";
  for( Net &net : nets ) file << net << endl;
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

Block* const Router::getBlock( const vector<Block> &blocks , const string &name )
{
  auto it = find_if(  blocks.begin() , blocks.end() ,
                      [&]( const Block &block ) { return block.name() == name; } );

  return ( it != blocks.end() ) ? const_cast<Block* const>( &( *it ) ) : nullptr;
}

int Router::getIndex( const vector<double> &array , double value )
{
  for( int i = 0 ; i < static_cast<int>( array.size() ) ; ++i )
     if( array[i] == value ) return i;
  return -1;
}

vector<vector<Grid>> Router::gridMap()
{
  vector<vector<Grid>> grids( mVsplit.size() - 1 , vector<Grid>( mHsplit.size() - 1 ) );

  for( const Group &group : groups )
  {
     int x = getIndex( mHsplit , group.left  () );
     int y = getIndex( mVsplit , group.bottom() );

     grids[y][x].setLabel( Grid::OBSTACLE );
  }

  for( const Block &block : blocks )
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

vector<Point> Router::connectedPin( Net &net )
{
  vector<Point> pins;

  for( const Pin &pin : net.pins() )
  {
     if(  ( mHsplit.front() <= pin.x() && pin.x() <= mHsplit.back() ) &&
          ( mVsplit.front() <= pin.y() && pin.y() <= mVsplit.back() ) )
     {
       unsigned int x;
       unsigned int y;

       for( x = 0 ; x < mHsplit.size() ; ++x )
          if( mHsplit[x] <= pin.x() ) break;
       for( y = 0 ; y < mVsplit.size() ; ++y )
          if( mVsplit[y] <= pin.y() ) break;

       pins.push_back( Point( x , y ) );
     }
  }

  return pins;
}
