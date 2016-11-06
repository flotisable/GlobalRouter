#include <iostream>
using namespace std;

#include "Router.h"

int main()
{
  Router router;
  
  router.readBlock( "../display_info/display0.txt" , "../two_stage_fullnets/test.constraints" );
  router.readNets( "../two_stage_fullnets/final.nets" );
  router.outputData( "routingReport.txt" );

  cin.get();
  return 0;
}
