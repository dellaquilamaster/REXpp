#ifndef ENERGYCONVERSIONTABLE_H
#define ENERGYCONVERSIONTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <TF1.h>
#include <Math/Interpolator.h>

class ConversionTableElement
{
public :
  ConversionTableElement();
  ~ConversionTableElement();
  
  void SetLimits(double Emin, double Emax);
  void SetPoints(int, double *, double *);
  
  int GetNPoints() const; 
  double GetInterpolation(double Edet) const;
  
  double GetMinEnergy() const;
  double GetMaxEnergy() const;

private :
  ROOT::Math::Interpolator * fTheConversionInterpolator;
  double fEmin;
  double fEmax;
  std::vector<double> fX;
  std::vector<double> fY;
  int fN;
  
};

class REXppConversionTable
{
public :
  REXppConversionTable();
  ~REXppConversionTable();
  
  int LoadDetectorConfiguration(const char *);
  int LoadEnergyConversionFile(const char *);
  
  double ReconstructQuantity(double Edet, int det, int stripf, int stripb) const;
  
private :
  std::vector<ConversionTableElement **> fTheTable;

};

#endif
