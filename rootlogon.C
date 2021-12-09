{  
  printf("Loading REX++ objects...\n");
  gROOT->ProcessLine(".L src/REXppConversionTable.cpp");
  gSystem->Load("libREXppRootEvent.so");
  
  printf("\n Beginning new ROOT session in the REX++ framework\n");

  TStyle *REXStyle  = new TStyle("REXStyle","The REX++ Style\n");
  printf("                                  \n");
  printf("   _____  ________   __           \n");
  printf("  |  __ \\|  ____\\ \\ / /_     _    \n");
  printf("  | |__) | |__   \\ V /| |_ _| |_  \n");
  printf("  |  _  /|  __|   > <_   _|_   _| \n");
  printf("  | | \\ \\| |____ / . \\|_|   |_|   \n");
  printf("  |_|  \\_\\______/_/ \\_\\           \n");
  printf("                                  \n\n");

  // from ROOT plain style                                                      
  REXStyle->SetCanvasBorderMode(0);
//   REXStyle->SetPadBorderMode(0);
//   REXStyle->SetPadColor(0);
//   REXStyle->SetCanvasColor(0);
  REXStyle->SetTitleColor(kBlue,"xyz");
//   REXStyle->SetStatColor(0);
  REXStyle->SetPalette(kBird);
  REXStyle->SetLabelSize(0.04,"xyz");                    
  REXStyle->SetTitleSize(0.04,"xyz");                    

  // default canvas positioning                                                 
  REXStyle->SetCanvasDefX(900);
  REXStyle->SetCanvasDefY(20);
  REXStyle->SetCanvasDefH(550);
  REXStyle->SetCanvasDefW(700);
  REXStyle->SetPadBottomMargin(0.1);
  REXStyle->SetPadTopMargin(0.1);
  REXStyle->SetPadLeftMargin(0.125);
  REXStyle->SetPadRightMargin(0.125);
  REXStyle->SetPadRightMargin(0.125);

  REXStyle->SetPadTickX(1);
  REXStyle->SetPadTickY(1);

  REXStyle->SetFrameBorderMode(0);
  REXStyle->SetFrameLineWidth(2);
  REXStyle->SetFrameFillColor(kCyan);

  gROOT->SetStyle("REXStyle"); //uncomment to set this style   
}
