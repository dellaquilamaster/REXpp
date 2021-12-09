#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <TVector3.h>
#include <TLorentzVector.h>

#include <nuclear_masses.h>

int main(int argc, char ** argv)
{
  nuclear_masses NucData("LISETools/input/masses.conf");
  
  if(argc!=2) {
    printf("Error, no beam energy inputed. Aborting!\n");
    exit(1);
  }
  
  const double BeamEnergy=atof(argv[1]);
  
  const int Zbeam=10;
  const int Abeam=20;
  const int Ztarget=2;
  const int Atarget=4;
  const int Zcompound=Zbeam+Ztarget;
  const int Acompound=Abeam+Atarget;
  
  const double MassBeam=NucData.get_mass_Z_A(Zbeam,Abeam); //mass of the beam in MeV/c2
  const double MassTarget=NucData.get_mass_Z_A(Ztarget,Atarget); // mass of the target in MeV/c2
  const double MassCompound_gs=NucData.get_mass_Z_A(Zcompound,Acompound); //mass of the compound in its ground state in MeV/c2
  
  const double BeamMomentumModuleInit=sqrt(pow(BeamEnergy+MassBeam,2)-pow(MassBeam,2)); //Initial module of the beam momentum
  const double Ethr = MassBeam+MassTarget-MassCompound_gs;
  const TVector3 BeamMomentumInit=BeamMomentumModuleInit*TVector3(0,0,1);
  const TLorentzVector BeamMomentumEnergyInit(BeamMomentumInit,BeamEnergy+MassBeam); //Initial beam Momentum/Energy quadri-vector
  const TLorentzVector TargetMomentumEnergy(0.,0.,0.,MassTarget); // Target Momentum/Energy quadri-vector
  double ExAvailable = (BeamMomentumEnergyInit+TargetMomentumEnergy).M()-MassCompound_gs; // Excitation energy available in the collision
  
  printf("%.6f\n", ExAvailable);
  
  return 0;
}
