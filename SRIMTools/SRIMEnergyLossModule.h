#ifndef SRIMENERGYLOSSMODULE_H
#define SRIMENERGYLOSSMODULE_H

#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <string>
#include <math.h>
#include <cmath>
#include <Math/Interpolator.h>

/* SRIMEnergyLossModule class
 * created by Daniele Dell'Aquila
 * 30/03/2020 Class similar to EnergyLossModule but for SRIM data.
 * SRIM is more accurate for energy loss calculations in gaseous materials.
 *
 * The class uses text files produced by SRIM (dE/dx (keV/um) plots and Range plots).
 * We use an associative map to identify the particular particle / material combination
 * for example Z=1 A=1 in gas He at 600mbar T=273K will be identified by the key "SRIM_Z01_A01_gas_He600mbar273.00K".
 * To see what is available please check the folder "./input".
 *
 */

class SRIMEnergyLossModule
{
public:
  SRIMEnergyLossModule(const char * program_path="./");
  ~SRIMEnergyLossModule();

  int LoadEnergyLossFile(const char *);
  void SetEnergyLossPrecision(double precision=0.001);
  void Clear();

  double GetEnergyLoss(int Z, int A, double Einc, const char * material, double thickness_um);                                              //Calculates the energy loss in a given material
  double GetEnergyLossGas(int Z, int A, double Einc, const char * material, double pressure, double temperature, double thickness_um);      //Calculates the energy loss in a given gaseous material

  double GetRangeGas(int Z, int A, double Einc, const char * material, double pressure, double temperature);                                //Calculates the range of a particle in a given gaseous material in um
  double GetRangeLongitudinalStragglingGas(int Z, int A, double Einc, const char * material, double pressure, double temperature);          //Calculates the longitudinal range straggling of a particle in a given gaseous material in um
  double GetRangeLateralStragglingGas(int Z, int A, double Einc, const char * material, double pressure, double temperature);               //Calculates the lateral range straggling of a particle in a given gaseous material in um

  double GetRange(int Z, int A, double Einc, const char * material);                                                                        //Calculates the range of a particle in a given material in um
  double GetRangeLongitudinalStraggling(int Z, int A, double Einc, const char * material);                                                  //Calculates the longitudinal range straggling of a particle in a given material in um
  double GetRangeLateralStraggling(int Z, int A, double Einc, const char * material);                                                       //Calculates the lateral range straggling of a particle in a given material in um

private:
  std::map<std::string, std::vector <double>> ParticleEnergy; //MeV
  std::map<std::string, std::vector <double>> SrimELoss;   //MeV/micron
  std::map<std::string, std::vector <double>> SrimRange; //micron
  std::map<std::string, std::vector <double>> SrimRangeLongitudinalStraggling; //micron
  std::map<std::string, std::vector <double>> SrimRangeLateralStraggling; //micron

  std::map<std::string,double> Emax; //MeV
  std::map<std::string,double> Emin; //MeV

  std::map<std::string, ROOT::Math::Interpolator *> SplineInterpolator;
  std::map<std::string, ROOT::Math::Interpolator *> RangeSplineInterpolator;
  std::map<std::string, ROOT::Math::Interpolator *> RangeLongitudinalStragglingSplineInterpolator;
  std::map<std::string, ROOT::Math::Interpolator *> RangeLateralStragglingSplineInterpolator;
  std::map<std::string, ROOT::Math::Interpolator *> EnergyFromRangeSplineInterpolator;

  std::string fDataPath;

  double fEnergyLossPrecision; //Lower is this number higher is the precision (slower calculation). Precision in MeV.
};

#endif
