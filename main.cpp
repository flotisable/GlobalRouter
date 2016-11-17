#include <iostream>
using namespace std;

#include "MazeRouter.h"
#include "Router.h"

#define FLOTISABLE_WIN10

int main()
{
#ifdef FLOTISABLE_LINUX
  const string dirPath      = "/home/flotisable/Collage/ProjectImplimentation/";
  const string displayPath  = dirPath + "display_info/";
  const string twoStagePath = dirPath + "TwoStageFullnets/";
#else
#ifdef FLOTISABLE_WIN10
  const string dirPath      = "f:/flotisable/Program/c++/Collage/TwoStage/";
  const string displayPath  = dirPath + "display_info/";
  const string twoStagePath = dirPath + "two_stage_fullnets/";
#else
  const string dirPath      = "";
  const string displayPath  = dirPath + "";
  const string twoStagePath = dirPath + "";
#endif
#endif

  MazeRouter  routingEngine;
  Router      router;

  router.setRouter( &routingEngine );

  router.readBlock( displayPath + "display0.txt" , twoStagePath + "test.constraints" );
  router.readNets( twoStagePath + "final.nets" );
  if( router.route() ) router.outputData( dirPath + "routingReport.txt" );

  cin.get();
  return 0;
}
