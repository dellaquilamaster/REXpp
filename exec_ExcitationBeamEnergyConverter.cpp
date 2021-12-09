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
    printf("Error, no excitation energy inputed. Aborting!\n");
    exit(1);
  }
  
  const double ExcitationEnergy=atof(argv[1]);
  
  const int Zbeam=10;
  const int Abeam=20;
  const int Ztarget=2;
  const int Atarget=4;
  const int Zcompound=Zbeam+Ztarget;
  const int Acompound=Abeam+Atarget;
  
  const double MassBeam=NucData.get_mass_Z_A(Zbeam,Abeam); //mass of the beam in MeV/c2
  const double MassTarget=NucData.get_mass_Z_A(Ztarget,Atarget); // mass of the target in MeV/c2
  const double MassCompound_gs=NucData.get_mass_Z_A(Zcompound,Acompound); //mass of the compound in its ground state in MeV/c2
  
  const double BeamMomentumModule=sqrt((1./(4.*pow(MassTarget,2)))*(-2*pow(MassTarget,2)*pow(MassBeam,2)+pow(ExcitationEnergy,2)*(pow(ExcitationEnergy,2)+4*pow(MassCompound_gs,2)+4*MassCompound_gs*ExcitationEnergy)
                                       +pow(MassCompound_gs,4)+pow(MassBeam,4)+pow(MassTarget,4)-2*pow(MassCompound_gs,2)*pow(MassBeam,2)-2*pow(MassCompound_gs,2)*pow(MassTarget,2)
                                       +2*ExcitationEnergy*(ExcitationEnergy+2*MassCompound_gs)*(pow(MassCompound_gs,2)-pow(MassBeam,2)-pow(MassTarget,2)))); //Module of the beam momentum calculated from the conservation of energy
  const double BeamEnergy=sqrt(pow(BeamMomentumModule,2)+pow(MassBeam,2))-MassBeam;
  
  printf("%.6f\n", BeamEnergy);
  
  return 0;
}
