/*
 * This macro shows the kinematic lines related to a certain reaction
 * as a function of the excitation energy of the compound nucleus (incident energy is equivalent)
 * 
 */

RelativisticKinematics KinematicModule;
nuclear_masses MassesModule("LISETools/input/masses.conf");

// x[0] = theta (deg)
// par[0] = Ebeam
// par[1] = Zbeam
// par[2] = Abeam
// par[3] = Ztarget
// par[4] = Atarget
// par[5] = Zparticle
// par[6] = Aparticle
// par[7] = Exparticle
// par[8] = Exother
double KinematicLine (double *x, double *par)
{
  double MassProjectileuma=MassesModule.get_mass_Z_A_uma(par[1],par[2]);
  double MassTargetuma=MassesModule.get_mass_Z_A_uma(par[3],par[4]);
  double EnergyPerNucleonProjectile=par[0]/MassProjectileuma;
  double AngleInRadiants=x[0]*TMath::DegToRad();
  
  return KinematicModule.GetEnergyTwoBodyEjectileReaction(AngleInRadiants, EnergyPerNucleonProjectile, MassProjectileuma, MassTargetuma, MassTargetuma, MassProjectileuma, par[7], par[8]);
}

void macro_EnergyEvolutionKinematicLines()
{
  const int Zbeam=10;
  const int Abeam=20;
  const int Ztarget=2;
  const int Atarget=4;
  const int Zparticle=2;
  const int Aparticle=4;
  const double Exparticle=0.; //excitation energy of the emitted particles we are looking
  const double Exother=0.; //excitation energy of the recoiling particle
  
  const double Ethr = MassesModule.get_mass_Z_A(Zbeam,Abeam)+MassesModule.get_mass_Z_A(Ztarget,Atarget)-MassesModule.get_mass_Z_A(Zbeam+Ztarget,Abeam+Atarget);
  const double ELabToCm = MassesModule.get_mass_Z_A(Ztarget,Atarget)/(MassesModule.get_mass_Z_A(Zbeam,Abeam)+MassesModule.get_mass_Z_A(Ztarget,Atarget));
  
  
  std::vector<double> CompoundEx;
  CompoundEx.push_back(17.5);
  CompoundEx.push_back(17.0);
  CompoundEx.push_back(16.0);
  CompoundEx.push_back(15.0);
  
  TCanvas * c1 = new TCanvas("c1","",800,600);
  
  double parameters[] = {0.,Zbeam,Abeam,Ztarget,Atarget,Zparticle,Aparticle,Exparticle,Exother};
  
  TLegend * legendario = new TLegend(0.7,0.7,0.9,0.9);
  
  for(int i=0; i<CompoundEx.size(); i++) {
    TF1 * TheFunction = new TF1 (Form("Func_%d",i), KinematicLine,0,90,9);
    TheFunction->SetParameters(parameters);
    TheFunction->SetParameter(0, (CompoundEx[i]-Ethr)/ELabToCm);
    TheFunction->SetLineColor(i+1);
    TheFunction->Draw(i>0 ? "SAME" : "");
    TheFunction->GetXaxis()->SetTitle("#theta (deg)");
    TheFunction->SetTitle("Kinematic lines");
    legendario->AddEntry(TheFunction,Form("Ex=%.1f MeV", CompoundEx[i]),"L");
  }
  
  legendario->Draw();
  
  return;
}
