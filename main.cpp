#include <iostream>
using namespace std;

#include "MazeRouter.h"
#include "Router.h"

int main()
{
  MazeRouter  routingEngine;
  /*Router      router;
  
  router.setRouter( &routingEngine );
  
  router.readBlock( "../display_info/display4.txt" , "../two_stage_fullnets/test.constraints" );
  router.readNets( "../two_stage_fullnets/final.nets" );
  router.route();
  router.outputData( "routingReport.txt" );*/
  
  vector<vector<Grid>> grids( 3 , vector<Grid>( 5 ) );
  
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
  
  cout << net << endl;

  cin.get();
  return 0;
}
