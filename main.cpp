#include <iostream>
using namespace std;

#include "Router.h"

int main()
{
  Router router;
  
  router.readBlock( "display_info/display0.txt" );
  router.outputData( "routingReport.txt" );

  cin.get();
  return 0;
}
