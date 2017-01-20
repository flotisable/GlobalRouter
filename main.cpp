#include <iostream>
#include <fstream>
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
#ifdef FLOTISABLE_WIN7
  const string dirPath      = "D:/ProjectImplimentation/";
  const string displayPath  = dirPath + "display_info/";
  const string twoStagePath = dirPath + "two_stage_fullnets/";
#else
  const string dirPath      = "";
  const string displayPath  = dirPath + "";
  const string twoStagePath = dirPath + "";
#endif
#endif
#endif

  MazeRouter  routingEngine;
  Router      router;

  router.setMaxLayer( 6 );
  router.setRouter( &routingEngine );

  try
  {
    router.readBlock( displayPath + "display0.txt" , twoStagePath + "test.constraints" );
    router.readNets( twoStagePath + "new_final.nets" );
    router.route();
    router.outputData( dirPath + "routingReport.txt" );
  }
  catch( const Router::FileOpenError &error )
  {
    cerr << "can not open file : " << error.what() << endl;
  }
  catch( const Router::NetCannotRoute &error )
  {
    cerr << "some nets can't be route!\n";
    router.outputData( dirPath + "routingReportError.txt" );
  }
  catch( const Router::RoutingEngineError &error )
  {
    cerr << "RoutingEngineError : " << error.engineName() << "::" << error.errorName() << endl;
  }

  cin.get();
  return 0;
}
