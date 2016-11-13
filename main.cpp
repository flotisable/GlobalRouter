#include <iostream>
using namespace std;

#include "MazeRouter.h"
#include "Router.h"

#define FLOTISABLE_LINUX

int main()
{
#ifdef FLOTISABLE_LINUX
  const string dirPath      = "/home/flotisable/Collage/ProjectImplimentation/";
  const string displayPath  = dirPath + "display_info/";
  const string twoStagePath = dirPath + "TwoStageFullnets/";
#else
  const string dirPath      = "";
  const string displayPath  = dirPath + "";
  const string twoStagePath = dirPath + "";
#endif

  MazeRouter  routingEngine;
  Router      router;
  
  router.setRouter( &routingEngine );
  
  router.readBlock( displayPath + "display0.txt" , twoStagePath + "test.constraints" );
  router.readNets( twoStagePath + "final.nets" );
  router.route();
  router.outputData( dirPath + "routingReport.txt" );
  
  /*vector<vector<Grid>> grids( 3 , vector<Grid>( 5 ) );
  
  grids[0][4].setLabel( Grid::OBSTACLE );
  grids[2][2].setLabel( Grid::OBSTACLE );
  
  for( int i = static_cast<int>( grids.size() ) - 1 ; i >= 0 ; --i )
  {
     for( const Grid &grid : grids[i] )
        switch( grid.label() )
        {
          case Grid::SPACE:     cout << "0"; break;
          case Grid::OBSTACLE:  cout << "1";  break;
          default: break;
        }
     cout << endl;
  }
  
  vector<Point> pins;
  
  pins.push_back( Point( 0 , 0 ) );
  pins.push_back( Point( 3 , 2 ) );
  
  Net net;
  
  routingEngine.setGrids( grids );
  routingEngine.setPins( pins );
  routingEngine.route();
  routingEngine.saveNet( net );
  
  cout << net << endl;*/

  cin.get();
  return 0;
}
