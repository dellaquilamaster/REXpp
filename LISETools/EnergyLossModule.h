#ifndef ENERGYLOSSMODULE_H
#define ENERGYLOSSMODULE_H

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

#include "nuclear_masses.h"

/* EnergyLossModule class
 * created by Daniele Dell'Aquila
 * This class has been improved on March 26th 2019, including fast methods for energy loss calculation
 * and to calculate the original energy of a particle from the energy loss or the energy residual.
 *
 * The class uses text files produced by LISE (dE/dx (MeV/um) plots and Range plots).
 * We use an associative map to identify the particular particle / material combination
 * for example Z=1 A=1 in CH2 will be identified by the key "LISE_ELoss_Z01_A01_CH2".
 * To see what is available please check the folder "./input".
 * To add a new material follow an existing example. Remember to erase or compile with "*" the first row of the dat file from LISE.
 *
 */

// NUMBER OF MODELS FOR ENERGY LOSS CALCULATIONS
static const int NUM_MODELS=6;
static const int NUM_RANGE_MODELS=9;

class EnergyLossModule
{
public:
  EnergyLossModule(const char * program_path="./");
  ~EnergyLossModule();

  int LoadEnergyLossFile(const char *);
  int LoadRangeFile(const char *);
  void SetEnergyLossPrecision(double precision=0.001); //Set the precision for energy loss calculation
  void Clear();

  //
  // Energy Loss in solid materials
  double GetEnergyLoss(int Z, int A, double Einc, const char * material, double thickness_um, int model=1);      //Calculates the energy loss in a given material
  double GetResidualEnergy(int Z, int A, double Eloss, const char * material, double thickness_um, int model=1); //Calculates the residual energy starting from the Eloss in a given material
  double GetIncidentEnergy(int Z, int A, double Eloss, const char * material, double thickness_um, int model=1); //Calculates the incident energy starting from the Eloss in a given material
  double GetIncidentEnergyFromResidual(int Z, int A, double Eres, const char * material, double thickness_um, int model=1); //Calculates the incident energy starting from the Eres after a given material

  double GetRangeFromEnergy(int Z, int A, double Einc, const char * material, int model=1);  //Calculates the particle range at a given energy in a given material
  double GetEnergyFromRange(int Z, int A, double range, const char * material, int model=1); //Calculates the particle energy to penetrate a given range into a given material
  
  double GetThicknessFromEnergyLoss(int Z, int A, double Einc, double Eloss, const char * material, int model=1); //Calculates the thickness of a given material from the energy loss of a particle of given energy
  //
  //
  
  //
  // Energy Loss in solid materials
  double GetEnergyLossGas(int Z, int A, double Einc, const char * material, double pressure, double temperature, double thickness_um, int model=1);      //Calculates the energy loss in a given material
  //
  //

  void DrawdEdx(int Z, int A, const char * material, int model=1);

private:
  std::map<std::string, std::vector <double>> ParticleEnergy; //MeV/u
  std::map<std::string, std::vector <double>> LiseELoss[NUM_MODELS];   //MeV/micron
  std::map<std::string, std::vector <double>> ParticleEnergyRange; //MeV/u
  std::map<std::string, std::vector <double>> LiseRange[NUM_RANGE_MODELS]; //micron

  std::map<std::string,double> Emax; //MeV/u
  std::map<std::string,double> Emin; //MeV/u
  std::map<std::string,double> ERangeMax; //MeV/u
  std::map<std::string,double> ERangeMin; //MeV/u
  std::map<std::string,double> RangeMin[NUM_RANGE_MODELS]; //micron
  std::map<std::string,double> RangeMax[NUM_RANGE_MODELS]; //micron

  std::map<std::string, ROOT::Math::Interpolator *> SplineInterpolator[NUM_MODELS];
  std::map<std::string, ROOT::Math::Interpolator *> RangeSplineInterpolator[NUM_RANGE_MODELS];
  std::map<std::string, ROOT::Math::Interpolator *> EnergyFromRangeSplineInterpolator[NUM_RANGE_MODELS];

  nuclear_masses *NucData;

  std::string fDataPath;

  double fEnergyLossPrecision; //Precision in MeV for energy loss calculation. Lower is this number higher is the precision (slower calculation)
};

#endif
