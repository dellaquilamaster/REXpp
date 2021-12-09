#include "../include/REXppConversionTable.h"

//____________________________________________________
ConversionTableElement::ConversionTableElement() :
fTheConversionInterpolator(0),
fEmin(0),
fEmax(0),
fX(0),
fY(0),
fN(0)
{}

//____________________________________________________
ConversionTableElement::~ConversionTableElement()
{}

//____________________________________________________
void ConversionTableElement::SetLimits(double Emin, double Emax)
{
  fEmin=Emin;
  fEmax=Emax;
}

//____________________________________________________
void ConversionTableElement::SetPoints(int n_points, double * Edet_pointer, double * Ex_pointer)
{
  for(int i=0; i<n_points; i++) {
    fX.push_back(Edet_pointer[i]);
    fY.push_back(Ex_pointer[i]);
    fN++;
  }
  
  fTheConversionInterpolator = new ROOT::Math::Interpolator(fX,fY);
    
  return;
}

//____________________________________________________
double ConversionTableElement::GetMinEnergy() const
{
  return fEmin; 
}

//____________________________________________________
double ConversionTableElement::GetMaxEnergy() const
{
  return fEmax; 
}

//____________________________________________________
int ConversionTableElement::GetNPoints() const
{
  return fN; 
}

//____________________________________________________
double ConversionTableElement::GetInterpolation(double Edet) const
{
  return Edet>=fEmin && Edet<=fEmax ? fTheConversionInterpolator->Eval(Edet) : -9999; 
}

//____________________________________________________
REXppConversionTable::REXppConversionTable()
{}

//____________________________________________________
REXppConversionTable::~REXppConversionTable()
{}

//____________________________________________________
int REXppConversionTable::LoadDetectorConfiguration(const char * file_config)
{
  //NOTE: for now the configuration of detetector is fixed.
  //We have 8 detectors, each containing 16x16 strips
  //TEMPORARY
  for(int i=0; i<8; i++) {
    ConversionTableElement ** NewDetector = (ConversionTableElement **) new ConversionTableElement *[16];
    for(int j=0; j<16; j++) {
      NewDetector[j]= new ConversionTableElement[16];
    }
    fTheTable.push_back(NewDetector);
  }
  
  return 8;
  //TEMPORARY
}

//____________________________________________________
int REXppConversionTable::LoadEnergyConversionFile(const char * file_name)
{
  std::ifstream FileIn(file_name);
  if(!FileIn.is_open()) {
    return -1;
  }
  int NRead=0;

  while (!FileIn.eof())
  {
    std::string LineRead;
    std::getline(FileIn, LineRead);

    std::string LineReadCommentLess (LineRead.substr(0, LineRead.find("*")));

    if(LineReadCommentLess.empty()) continue;
    if(LineReadCommentLess.find_first_not_of(' ') == std::string::npos) continue;

    std::istringstream LineStream(LineReadCommentLess);

    int num_detector;
    int numstripf;
    int numstripb;
    double Emin;
    double Emax;
    int num_points;
    std::map<double, double> NewList; 
    std::vector<double> Edet_points;
    std::vector<double> Ex_points;

    LineStream >> num_detector >> numstripf >> numstripb >> Emin >> Emax >> num_points;
    
    if(num_points<3) continue;
    
    for(int i=0; i<num_points; i++) {
      double Edet_read;
      double Ex_read;
      LineStream>>Edet_read>>Ex_read;
      NewList[Edet_read]=Ex_read;
    }
    
    //
    //Inserting entries in an sorted vector (Increasing Ex)
    for(std::map<double,double>::iterator it=NewList.begin(); it!=NewList.end(); it++) {
      Edet_points.push_back(it->first);
      Ex_points.push_back(it->second);
    }
    //
    
    (fTheTable[num_detector-1])[numstripf][numstripb].SetLimits(Emin,Emax);
    (fTheTable[num_detector-1])[numstripf][numstripb].SetPoints(num_points,Edet_points.data(),Ex_points.data());
    
    NRead++;
  }
  
  return NRead;
}

//____________________________________________________
double REXppConversionTable::ReconstructQuantity(double Edet, int num_detector, int numstripf, int numstripb) const
{  
  return (fTheTable[num_detector-1])[numstripf][numstripb].GetInterpolation(Edet);
}

