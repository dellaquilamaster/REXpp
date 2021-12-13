const double fStartTime=clock();

void PrintPercentage(int jentry, int nentries)
{
  double time_elapsed = (double)(clock() - fStartTime)/CLOCKS_PER_SEC;
  std::cout << "  Percentage = " << std::fixed << std::setprecision(1) << std::setw(5) << (100*double(jentry)/nentries) << " %";
  std::cout << "   [";
  int printindex=0;
  for(; printindex<int(100*double(jentry)/nentries); printindex+=5) std::cout << "=";
  for(; printindex<100; printindex+=5) std::cout << " ";
  std::cout << "]   " << "elapsed time " << std::setprecision(1) <<
  (time_elapsed<60 ? time_elapsed : (time_elapsed<3600 ? time_elapsed/60 : time_elapsed/3600)) <<
  (time_elapsed<60 ? " s; " : (time_elapsed<3600 ? " m; " : " h; "));
  if(jentry>0) {
    double time_remaining = (time_elapsed/jentry)*(nentries-jentry);
    std::cout << " estimated remaining time " << std::setprecision(1) <<
    (time_remaining<60 ? time_remaining : (time_remaining<3600 ? time_remaining/60 : time_remaining/3600)) <<
    (time_remaining<60 ? " s      " : (time_remaining<3600 ? " m      " : " h      "));
  }
  std::cout << "\r";
  std::cout.flush();
}

void macro_ReconstructionProcessTwoBodyDevel()
{
  //
  const double GasPressure=355; //mbar //In the LNL experiment: 873 mbar, 869 mbar, 579 mbar, 253 mbar, 243 mbar, 355 mbar, 690 mbar
  const std::string GasMaterial("He"); // gas material
  const std::string WindowMaterial("Havar"); //material of the entrance gas window
  const double WindowThickness=2.0; //Thickness of the solid window in um
  const double BeamEnergy=58; //MeV //In the LNL experiment: 69 MeV, 59 MeV, 48 MeV
  const int Zbeam=10;
  const int Abeam=20;
  const int Ztarget=2;
  const int Atarget=4;
  const int Zejectile=2;
  const int Aejectile=4;
  const int Zresidual=Zbeam+Ztarget-Zejectile;
  const int Aresidual=Abeam+Atarget-Aejectile;
  const int Zcompound=Zbeam+Ztarget;
  const int Acompound=Abeam+Atarget;
  const std::string EntranceChannel("Ne20_He4");
  const std::string ExitChannel("Ne20_He4");
  const double Exresidual=0.; // Excitation energy populated in the heavy residual //In the LNL experiment is 20Ne
  //
  const int num_detectors=8;
  const int num_strips_front=16;
  const int num_strips_back=16;
  //
  
  //
  //Creating conversion table for 20Ne+4He->20Ne+4He, 4He detected
  REXppConversionTable TheConversionModule_Elastic_Ejectile;
  TheConversionModule_Elastic_Ejectile.LoadDetectorConfiguration(""); //Building detector configuration 
  TheConversionModule_Elastic_Ejectile.LoadEnergyConversionFile(Form("output/REXpp_ConversionParameters_%s_%s_Z%02d_A%02d_%s%.2fum_%.0fmbar_%.0fMeV.dat",EntranceChannel.c_str(),ExitChannel.c_str(),Zejectile, Aejectile,  WindowMaterial.c_str(),WindowThickness,GasPressure,BeamEnergy)); //Retrieving conversion functions
  //
  //Creating conversion table for 20Ne+4He->20Ne+4He, 20Ne detected (high energy solution)
  REXppConversionTable TheConversionModule_Elastic_Residual_Hi;
  TheConversionModule_Elastic_Residual_Hi.LoadDetectorConfiguration(""); //Building detector configuration 
  TheConversionModule_Elastic_Residual_Hi.LoadEnergyConversionFile(Form("output/REXpp_ConversionParameters_%s_%s_Z%02d_A%02d_%s%.2fum_%.0fmbar_%.0fMeV_Hi.dat",EntranceChannel.c_str(),ExitChannel.c_str(),Zresidual, Aresidual,  WindowMaterial.c_str(),WindowThickness,GasPressure,BeamEnergy)); //Retrieving conversion functions
  //
  //Creating conversion table for 20Ne+4He->20Ne+4He, 20Ne detected (low energy solution)
  REXppConversionTable TheConversionModule_Elastic_Residual_Low;
  TheConversionModule_Elastic_Residual_Low.LoadDetectorConfiguration(""); //Building detector configuration 
  TheConversionModule_Elastic_Residual_Low.LoadEnergyConversionFile(Form("output/REXpp_ConversionParameters_%s_%s_Z%02d_A%02d_%s%.2fum_%.0fmbar_%.0fMeV_Low.dat",EntranceChannel.c_str(),ExitChannel.c_str(),Zresidual, Aresidual,  WindowMaterial.c_str(),WindowThickness,GasPressure,BeamEnergy)); //Retrieving conversion functions
  //
  
  //
  //Initializing Constants
  nuclear_masses NucData("LISETools/input/masses.conf"); //Loading nuclear masses from up-to-date database
  //
  // Masses of the nuclei involved
  const double MassBeam=NucData.get_mass_Z_A(Zbeam,Abeam); //mass of the beam in MeV/c2
  const double MassTarget=NucData.get_mass_Z_A(Ztarget,Atarget); // mass of the target in MeV/c2
  const double MassEjectile=NucData.get_mass_Z_A(Zejectile,Aejectile); //mass of the light ejectile in MeV/c2
  const double MassResidual=NucData.get_mass_Z_A(Zresidual,Aresidual)+Exresidual; //mass of the heavy residual in MeV/c2
  const double MassCompound_gs=NucData.get_mass_Z_A(Zcompound,Acompound); //mass of the compound in its ground state in MeV/c2
  //
  const double BeamMomentumModuleInit=sqrt(pow(BeamEnergy+MassBeam,2)-pow(MassBeam,2)); //Initial module of the beam momentum
  const double ParticleEmissionThreshold=MassEjectile+MassResidual-MassCompound_gs;
  const TVector3 BeamMomentumInit=BeamMomentumModuleInit*TVector3(0,0,1);
  const TLorentzVector BeamMomentumEnergyInit(BeamMomentumInit,BeamEnergy+MassBeam); //Initial beam Momentum/Energy quadri-vector
  const TLorentzVector TargetMomentumEnergy(0.,0.,0.,MassTarget); // Target Momentum/Energy quadri-vector
  //
  
  //
  //Opening data
  const std::string FileInName(Form("output/REXpp_%s_%s_%s%.2fum_%.0fmbar_%.0fMeV_*.root",EntranceChannel.c_str(),ExitChannel.c_str(),WindowMaterial.c_str(),WindowThickness,GasPressure,BeamEnergy));
  //
  TChain * TreeDetected = new TChain("t");
  TreeDetected->Add(FileInName.c_str());
  TreeDetected->SetMakeClass(1);
  Int_t cluster_fmulti;
  Int_t cluster_fmulti_detected;
  Double_t cluster_fEx;
  Bool_t cluster_fIsDetected[10];
  Int_t cluster_fnumtel[10];
  Int_t cluster_fnumstripf[10];
  Int_t cluster_fnumstripb[10];
  Double_t cluster_fKinEnergy[10];
  Double_t cluster_fKinEnergyOrigin[10];
  Double_t cluster_fKinEnergyOriginCms[10];
  Double_t cluster_fThetaOriginCms[10];
  Int_t cluster_fZ[10];
  Int_t cluster_fA[10];
  TreeDetected->SetBranchAddress("cluster.fmulti",&cluster_fmulti);
  TreeDetected->SetBranchAddress("cluster.fmulti_detected",&cluster_fmulti_detected);
  TreeDetected->SetBranchAddress("cluster.fEx",&cluster_fEx);
  TreeDetected->SetBranchAddress("cluster.fnumtel",cluster_fnumtel);
  TreeDetected->SetBranchAddress("cluster.fIsDetected",cluster_fIsDetected);
  TreeDetected->SetBranchAddress("cluster.fnumstripf",cluster_fnumstripf);
  TreeDetected->SetBranchAddress("cluster.fnumstripb",cluster_fnumstripb);
  TreeDetected->SetBranchAddress("cluster.fKinEnergy",cluster_fKinEnergy);
  TreeDetected->SetBranchAddress("cluster.fKinEnergyOrigin", cluster_fKinEnergyOrigin); //Kinetic energy of the particle as emitted by the compound
  TreeDetected->SetBranchAddress("cluster.fKinEnergyOriginCms", cluster_fKinEnergyOriginCms); //Kinetic energy of the particle as emitted by the compound in the Cms
  TreeDetected->SetBranchAddress("cluster.fThetaOriginCms", cluster_fThetaOriginCms); //Theta angle of the particle as emitted by the compound in the Cms
  TreeDetected->SetBranchAddress("cluster.fZ",cluster_fZ); //Charge of the particle
  TreeDetected->SetBranchAddress("cluster.fA",cluster_fA); //Mass of the particle
  TreeDetected->SetBranchStatus("*",false);
  TreeDetected->SetBranchStatus("cluster.fmulti", true);
  TreeDetected->SetBranchStatus("cluster.fmulti_detected", true);
  TreeDetected->SetBranchStatus("cluster.fEx", true);
  TreeDetected->SetBranchStatus("cluster.fnumtel", true);
  TreeDetected->SetBranchStatus("cluster.fIsDetected", true);
  TreeDetected->SetBranchStatus("cluster.fnumstripf", true);
  TreeDetected->SetBranchStatus("cluster.fnumstripb", true);
  TreeDetected->SetBranchStatus("cluster.fKinEnergy", true);
  TreeDetected->SetBranchStatus("cluster.fKinEnergyOrigin", true);
  TreeDetected->SetBranchStatus("cluster.fThetaOriginCms", true);
  TreeDetected->SetBranchStatus("cluster.fZ", true);
  TreeDetected->SetBranchStatus("cluster.fA", true);
  //
  
  //Opening output file
  const std::string FileOutName(Form("/run/media/daniele/WD_IRB/simulations/2019_Mg24/REXpp_ReconstructionProcess_%s_%s_%s%.2fum_%.0fmbar_%.0fMeV_TwoBody.root", EntranceChannel.c_str(),ExitChannel.c_str(),WindowMaterial.c_str(),WindowThickness,GasPressure,BeamEnergy));
  //
  TFile * FileOut = new TFile(FileOutName.c_str(),"RECREATE");
  TTree * TreeOut = new TTree ("t", "Tree with event reconstruction");
  Int_t event_ID;
  Int_t NumTelescope_ejectile;
  Int_t NumStripf_ejectile;
  Int_t NumStripb_ejectile;
  Double_t KinEnergyDetected_ejectile;
  Double_t KinEnergy_ejectile;
  Double_t Ex_reconstructed_ejectile;
  Double_t Theta_ejectile;
  Double_t Phi_ejectile;
  Double_t ThetaCms_ejectile;
  Int_t NumTelescope_residual;
  Int_t NumStripf_residual;
  Int_t NumStripb_residual;
  Double_t KinEnergyDetected_residual;
  Double_t KinEnergy_residual;
  Double_t Ex_reconstructed_residual;
  Double_t Theta_residual;
  Double_t Phi_residual;
  Double_t ThetaCms_residual;
  Double_t Ex_reconstructed;
  TreeOut->Branch("NumTelescope_ejectile",&NumTelescope_ejectile,"NumTelescope_ejectile/I");
  TreeOut->Branch("NumStripf_ejectile",&NumStripf_ejectile,"NumStripf_ejectile/I");
  TreeOut->Branch("NumStripb_ejectile",&NumStripb_ejectile,"NumStripb_ejectile/I");
  TreeOut->Branch("KinEnergyDetected_ejectile",&KinEnergyDetected_ejectile,"KinEnergyDetected_ejectile/D");
  TreeOut->Branch("KinEnergy_ejectile",&KinEnergy_ejectile,"KinEnergy_ejectile/D");
  TreeOut->Branch("Ex_reconstructed_ejectile",&Ex_reconstructed_ejectile,"Ex_reconstructed_ejectile/D");
  TreeOut->Branch("Theta_ejectile",&Theta_ejectile,"Theta_ejectile/D");
  TreeOut->Branch("Phi_ejectile",&Phi_ejectile,"Phi_ejectile/D");
  TreeOut->Branch("ThetaCms_ejectile",&ThetaCms_ejectile,"ThetaCms_ejectile/D");
  TreeOut->Branch("NumTelescope_residual",&NumTelescope_residual,"NumTelescope_residual/I");
  TreeOut->Branch("NumStripf_residual",&NumStripf_residual,"NumStripf_residual/I");
  TreeOut->Branch("NumStripb_residual",&NumStripb_residual,"NumStripb_residual/I");
  TreeOut->Branch("KinEnergyDetected_residual",&KinEnergyDetected_residual,"KinEnergyDetected_residual/D");
  TreeOut->Branch("KinEnergy_residual",&KinEnergy_residual,"KinEnergy_residual/D");
  TreeOut->Branch("Ex_reconstructed_residual",&Ex_reconstructed_residual,"Ex_reconstructed_residual/D");
  TreeOut->Branch("Theta_residual",&Theta_residual,"Theta_residual/D");
  TreeOut->Branch("Phi_residual",&Phi_residual,"Phi_residual/D");
  TreeOut->Branch("ThetaCms_residual",&ThetaCms_residual,"ThetaCms_residual/D");
  TreeOut->Branch("Ex_reconstructed",&Ex_reconstructed,"Ex_reconstructed/D");
  TreeOut->SetAutoSave(500000);
  //
  
  //
  //Loop on data
  Long64_t nentries = TreeDetected->GetEntries();
  for(Long64_t ientry=0; ientry<nentries; ientry++) {
    
    if(ientry%100==0) {
      PrintPercentage(ientry, nentries);
    }
    
    TreeDetected->GetEntry(ientry);
    
    //
    event_ID=ientry;
    NumTelescope_ejectile=-1;
    NumStripf_ejectile=-1;
    NumStripb_ejectile=-1;
    Ex_reconstructed_ejectile=-9999;
    KinEnergyDetected_ejectile=-9999;
    KinEnergy_ejectile=-9999;
    Theta_ejectile=-9999;
    Phi_ejectile=-9999;
    ThetaCms_ejectile=-9999;
    NumTelescope_residual=-1;
    NumStripf_residual=-1;
    NumStripb_residual=-1;
    Ex_reconstructed_residual=-9999;
    KinEnergyDetected_residual=-9999;
    KinEnergy_residual=-9999;
    Theta_residual=-9999;
    Phi_residual=-9999;
    ThetaCms_residual=-9999;
    Ex_reconstructed=-9999;
    //
    
    //
    //We are analyzing two-particle coincidences
    if(cluster_fmulti_detected<2) continue;
    //
    
    //
    for(int i=0; i<cluster_fmulti; i++) {
      //Ejectile
      if(cluster_fIsDetected[i] && cluster_fZ[i]==Zejectile && cluster_fA[i]==Aejectile) {
        NumTelescope_ejectile=cluster_fnumtel[i];
        NumStripf_ejectile=cluster_fnumstripf[i];
        NumStripb_ejectile=cluster_fnumstripb[i];
        KinEnergyDetected_ejectile=cluster_fKinEnergy[i];
        Ex_reconstructed_ejectile=TheConversionModule_Elastic_Ejectile.ReconstructQuantity(cluster_fKinEnergy[i], cluster_fnumtel[i], cluster_fnumstripf[i], cluster_fnumstripb[i]);
      } else
      //Residual
      if(cluster_fIsDetected[i] && cluster_fZ[i]==Zresidual && cluster_fA[i]==Aresidual) {
        NumTelescope_residual=cluster_fnumtel[i];
        NumStripf_residual=cluster_fnumstripf[i];
        NumStripb_residual=cluster_fnumstripb[i];
        KinEnergyDetected_residual=cluster_fKinEnergy[i];
        if( (Ex_reconstructed_residual=TheConversionModule_Elastic_Residual_Hi.ReconstructQuantity(cluster_fKinEnergy[i], cluster_fnumtel[i], cluster_fnumstripf[i], cluster_fnumstripb[i]))<0 ) {
          Ex_reconstructed_residual=TheConversionModule_Elastic_Residual_Low.ReconstructQuantity(cluster_fKinEnergy[i], cluster_fnumtel[i], cluster_fnumstripf[i], cluster_fnumstripb[i]);
        }
      }
      //
      
      //
      // Decision on Excitation Energy and calculation of quantities
      if(Ex_reconstructed_ejectile>0 && Ex_reconstructed_residual>0 && fabs(Ex_reconstructed_ejectile-Ex_reconstructed_residual)<0.2) {
        Ex_reconstructed=Ex_reconstructed_ejectile; //We reconstruct the excitation energy as the average of the ones reconstructed from ejectile and residual
        KinEnergyDetected_ejectile=-9999;
        KinEnergyDetected_residual=-9999;
        Theta_ejectile=-9999;
        Theta_residual=-9999;
        Phi_ejectile=-9999;
        Phi_residual=-9999;
      }
      //
    }
    //
    
    //Fill the event event_ID
    TreeOut->Fill();
    
  }
  // End of loop on data
  
  //Saving Tree to TFile
  TreeOut->AutoSave();
  //
  
  //Closing TFile
  FileOut->Close();
  //  
  
  //
  printf("\n\nFile %s produced.\nThanks for using the REX++ reconstruction process, Bye!\n",FileOutName.c_str());
  //
  
  return;
}  
