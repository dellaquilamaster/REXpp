

#include <stdio.h>
#include <stdlib.h>

#include <REXppFramework.h>

int main (int argc, char ** argv)
{  
  //
  //Creation of the framework
  REXppFramework TheFramework;
  //
  
  //
  //Configuring the framework
  if(TheFramework.ConfigureFramework()<=0) {
    printf("Error: failed to configure the framework. Check configuration file. Aborting\n\n");
    exit(1);
  }
  //
  
  //
  //Printing configuration
  TheFramework.PrintConfiguration();
  //
  
  //
  //Reading input
  if(TheFramework.ReadInput(argc,argv)<2) {
    printf("Error: invalid input!\nPlease specify number of events and Ex.\nAborting.\n\n");
    exit(2);
  }
  //

  //
  //Output Tree Initialization
  TheFramework.InitTree();
  //
  
  //
  //Running the framework
  TheFramework.ProcessIterations();
  //
  
  //
  //Terminating the process
  TheFramework.EndProcess();
  //
  
  return 0; 
}
