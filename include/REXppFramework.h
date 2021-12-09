/* THE REX++ Framework
 * Created by: Daniele Dell'Aquila
 * e-mail:     daniele.dellaquila@irb.hr
 * 
 * The framework is intended as an extension to more complicated cases, and 
 * a more user friendly version using the C++ language, of the original 
 * REX simulation code by N. Curtis: N. Curtis and J. Walshe, Nucl. Instr. Meth. A 797 (2015) 44.
 * 
 * 05/12/2021
 * v2.0
 * This version now requires a fully working installation of UNISim-tool
 * detectors are imported from UNISim-tool
 * 
 * 30/05/2019
 * v1.0
 * Fully-debugged version
 * All information is read from inpunt file
 * 
 */

#ifndef REXPPFRAMEWORK_H
#define REXPPFRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <ctime>
#include <math.h>
#include <vector>
#include <map>
#include <TStyle.h>
#include <TEveManager.h>
#include <TRint.h>
#include <TEveStraightLineSet.h>
#include <TEveBox.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include <TRandom3.h>
#include <TMath.h>
#include <TF1.h>

#include <REXppRootEvent.h>
#include <nuclear_masses.h>
#include <SRIMEnergyLossModule.h>
#include <EnergyLossModule.h>
#include <RelativisticKinematics.h>
#include <UNISDetectionSetup.h>
#include <UNISDetectionUnit.h>
#include <UNISStripCluster.h>
#include <UNISStripDetector.h>
#include <UNISFaziaQuartet.h>
#include <UNISFaziaBlock.h>
#include <UNISLampWedgeDetector.h>
#include <UNISLampWedgeMMMDetector.h>
#include <UNISSiliconPhotoDiode.h>
#include <UNISStripSingleSidedDetector.h>
#include <UNISOscarTelescope.h>
#include <UNISCollimatedSilicon.h>

class REXppFramework
{
public :
  REXppFramework(); //Constructor
  REXppFramework(const char *); //Constructor
  ~REXppFramework(); //Destructor
  
  //
  // Configuration methods
  int ConfigureFramework(); //Lauch framework configuration
  void InitTree(); //Initialize output tree
  void SetEx(double); //Set the excitation energy
  void SetIterations(int); //Set the number of iterations
  void SetVerbose(bool opt=true); //Set verbose mode
  void SetGraphics(bool opt=true); //Enables openGL graphics
  //
  
  //
  //Methods to run the framework
  int ReadInput(int, char **); //Read input parameters from string
  void ProcessIterations(); //Process fNumEvents iterations of the framework
  void EndProcess(); //End iteration process, save tree to file and close file
  //
  
  //
  //Chech the configuration
  void PrintConfiguration() const;
  //
  
private :
  //
  //Framework Data Members
  const std::string fConfigurationFile;
  std::string fOutputFolder;
  std::string fOutputFileName;
  //
  
  //
  //Output Data
  TTree * TheTree;
  TFile * TheFile;
  //
  
  //
  //Physics constants
  const double fUMAToMeV;
  const double fcmToum;
  //
  int fNumEvents; //Number of framework iterations
  double fCompoundEx; //Escitation energy to reach in the compound nucleus (set by the void SetEx(double) method
  double fEffectiveCompoundEx; //Escitation energy reached in the compound nucleus after beam iterations
  double fGasPressure; //mbar //In the LNL experiment: 873 mbar, 869 mbar, 579 mbar, 253 mbar, 243 mbar, 355 mbar, 690 mbar
  double fGasTemperature; //T
  std::string fGasMaterial; // gas material
  std::string fWindowMaterial; //material of the entrance gas window
  double fWindowThickness; //Thickness of the solid window in um
  double fBeamEnergy; //MeV //In the LNL experiment: 69 MeV, 59 MeV, 48 MeV
  int fZbeam; // Charge of the beam
  int fAbeam; // Mass of the beam
  int fZtarget; //Charge of the target
  int fAtarget; // Mass of the target
  double fBeamCenterX; // position of the beam impinging on the entrance window
  double fBeamCenterY; // position of the beam impinging on the entrance window
  double fLimitX; // Limit of the useful volume along the X-axis (cm)
  double fLimitY; // Limit of the useful volume along the Y-axis (cm)
  double fLimitZ; // Limit of the useful volume along the Z-axis (cm)
  double fCalculationStep; // Calculation step (cm)
  double fEnergyLossPrecision; // Energy loss precision (lower is better but more time consuming)
  int fZejectile; // Charge of the light ejectile
  int fAejectile; // Mass of the light ejectile
  int fZresidual; // Charge of the heavy residual
  int fAresidual; // Mass of the heavy residual
  int fZcompound; // Charge of the compound nucleus
  int fAcompound; // Mass of the compound nucleus
  std::string fEntranceChannel; // The name of the entrance channel
  std::string fExitChannel; //The name of the exit channel
  double fExresidual; // Excitation energy populated in the heavy residual //In the LNL experiment is 20Ne
  double fDetectorThreshold; //Energy threshold of the detectors
  double fDetectorEres; // Energy resolution of the detectors
  bool fVerbose;
  bool fGraphics;
  bool fAdvancedGraphics;
  double fRandomSeed;
  double fStartTime;
  double fMassBeam; //mass of the beam in MeV/c2
  double fMassBeam_uma; //mass of the beam in amu
  double fMassTarget; // mass of the target in MeV/c2
  double fMassTarget_uma; // mass of the target in amu
  double fMassEjectile; //mass of the light ejectile in MeV/c2
  double fMassEjectile_uma; //mass of the light ejectile in amu
  double fMassResidual; //mass of the heavy residual in MeV/c2
  double fMassResidual_uma; //mass of the heavy residual in amu
  double fMassCompound; //mass of the compound in MeV/c2
  double fMassCompound_uma; //mass of the compound in amu
  double fMassCompound_gs; //mass of the compound in its ground state in MeV/c2
  //
  
  //
  //The detection setup
  UNISDetectionSetup * fExpSetup; 
  //
  
  //
  //The root event tools
  REXppRootEvent * fevt;
  //
  
  //
  //Modules
  TRandom3 * fRandomGen;
  nuclear_masses * fNucData;
  SRIMEnergyLossModule * fSRIMLossModule;
  EnergyLossModule * fLISELossModule;
  //
  
  //
  //Methods used by the configuration process to read the input file
  int ProcessSetCommand(const char *);
  int ProcessDefineCommand(const char *);
  int ProcessAddCommand(const char *);
  //
  
  //
  //Calculation methods
  void SteppingActionWindow(TLorentzVector & pre_momentum, TVector3 & pre_location, TLorentzVector & post_momentum, TVector3 & post_location);
  void SteppingActionGas(TLorentzVector & pre_momentum, TVector3 & pre_location, TLorentzVector & post_momentum, TVector3 & post_location, int Z, int A, double step_size);
  //
  
  //
  //Graphictools
  TRint * fApp;
  TEveStraightLineSet * TrackBeam;
  TEveStraightLineSet * TrackEjectile;
  TEveStraightLineSet * TrackResidual;
  //
  
  //
  //Utility methods
  void PrintPercentage(int, int) const;
  //
};

#endif
