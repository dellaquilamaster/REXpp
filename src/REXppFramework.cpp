#include <REXppFramework.h>

//____________________________________________________
REXppFramework::REXppFramework() : REXppFramework("config/REXpp.conf")
{}

//____________________________________________________
REXppFramework::REXppFramework(const char * file_config) :
fConfigurationFile(file_config),
fOutputFolder("./output/"),
fOutputFileName("output.root"),
TheTree(0),
TheFile(0),
fUMAToMeV(931.4936148),
fcmToum(10000),
fBeamCenterX(0.),
fBeamCenterY(0.),
fEnergyLossPrecision(0.001),
fExresidual(0.),
fDetectorEres(0.),
fVerbose(false),
fGraphics(false),
fAdvancedGraphics(false),
fDetectorThreshold(0.),
fRandomSeed(0),
fNucData(new nuclear_masses("LISETools/input/masses.conf")),
fSRIMLossModule(new SRIMEnergyLossModule("SRIMTools/input/")),
fLISELossModule(new EnergyLossModule("LISETools/input/")),
fApp(new TRint("The Unified-Simulation-tool", new int(), 0, 0, 0, kTRUE))
{}

//____________________________________________________
REXppFramework::~REXppFramework()
{}

//____________________________________________________
void REXppFramework::SetEx(double value)
{
  fCompoundEx=value;
}

//____________________________________________________
void REXppFramework::SetIterations(int value)
{
  fNumEvents=value;
}

//____________________________________________________
int REXppFramework::ConfigureFramework()
{
  std::ifstream FileIn(fConfigurationFile.c_str());
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
    
    std::string Command;
    
    LineStream >> Command;
    
    if(Command.compare("set")==0) {
      NRead+=ProcessSetCommand(LineReadCommentLess.c_str());
    } else if (Command.compare("define")==0) {
      NRead+=ProcessDefineCommand(LineReadCommentLess.c_str());
    } else if (Command.compare("add")==0) {
      NRead+=ProcessAddCommand(LineReadCommentLess.c_str());
    }
  }
  
  // Construction of the framework
  //
  fZcompound=fZbeam+fZtarget;
  fAcompound=fAbeam+fAtarget;
  fZresidual=fZcompound-fZejectile;
  fAresidual=fAcompound-fAejectile;
  //
  // Masses of the nuclei involved
  fMassBeam=fNucData->get_mass_Z_A(fZbeam,fAbeam); //mass of the beam in MeV/c2
  fMassBeam_uma=fNucData->get_mass_Z_A_uma(fZbeam,fAbeam); //mass of the beam in amu
  fMassTarget=fNucData->get_mass_Z_A(fZtarget,fAtarget); // mass of the target in MeV/c2
  fMassTarget_uma=fNucData->get_mass_Z_A_uma(fZtarget,fAtarget); // mass of the target in amu
  fMassEjectile=fNucData->get_mass_Z_A(fZejectile,fAejectile); //mass of the light ejectile in MeV/c2
  fMassEjectile_uma=fNucData->get_mass_Z_A_uma(fZejectile,fAejectile); //mass of the light ejectile in amu
  fMassResidual=fNucData->get_mass_Z_A(fZresidual,fAresidual)+fExresidual; //mass of the heavy residual in MeV/c2
  fMassResidual_uma=fNucData->get_mass_Z_A_uma(fZresidual,fAresidual)+fExresidual/fUMAToMeV; //mass of the heavy residual in amu
  fMassCompound=fNucData->get_mass_Z_A(fZcompound,fAcompound)+fCompoundEx; //mass of the compound in MeV/c2
  fMassCompound_uma=fNucData->get_mass_Z_A_uma(fZcompound,fAcompound)+fCompoundEx/fUMAToMeV; //mass of the compound in amu
  fMassCompound_gs=fNucData->get_mass_Z_A(fZcompound,fAcompound); //mass of the compound in its ground state in MeV/c2
  //
  
  //
  //Inizializing random generator
  fRandomGen = new TRandom3(fRandomSeed);
  gRandom->SetSeed(fRandomSeed);
  //
  
  return NRead;  
}

//____________________________________________________
int REXppFramework::ProcessSetCommand(const char * line)
{
  std::string InputLine(line);
  std::istringstream LineStream(InputLine);
  
  std::string Command;
  std::string WhatToSet;
  
  LineStream >> Command >> WhatToSet;
  
  std::string ValueToSet;
  
  LineStream>>ValueToSet;
  
  if(WhatToSet.compare("CALCULATION_STEP")==0) {
    fCalculationStep=std::stof(ValueToSet); 
  } else if(WhatToSet.compare("ENERGY_LOSS_PRECISION")==0) {
    fEnergyLossPrecision=std::stof(ValueToSet); 
  } else if(WhatToSet.compare("MOTHER_VOLUME")==0) {
    do {
      if(ValueToSet.find("-X")!=std::string::npos) {
        ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-X=")+3)); 
        fLimitX=std::stof(ValueToSet); 
      } else if(ValueToSet.find("-Y")!=std::string::npos) {
        ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-Y=")+3)); 
        fLimitY=std::stof(ValueToSet); 
      } else if(ValueToSet.find("-Z")!=std::string::npos) {
        ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-Z=")+3)); 
        fLimitZ=std::stof(ValueToSet); 
      }
    } while (LineStream>>ValueToSet);
  } else if(WhatToSet.compare("VERBOSE_MODE")==0) {
    fVerbose=ValueToSet.compare("true")==0 ? true : false;
  } else if(WhatToSet.compare("GRAPHICAL_MODE")==0) {
    if(ValueToSet.compare("light")==0) {
      fGraphics=true; 
    } else if(ValueToSet.compare("advanced")==0) {
      fGraphics=true;
      fAdvancedGraphics=true;
    }
    fGraphics=ValueToSet.compare("false")==0 ? false : true;
  } else if(WhatToSet.compare("OUTPUT_DIRECTORY")==0) {
    fOutputFolder.assign(ValueToSet.substr(ValueToSet.find("\"")+1,ValueToSet.find_last_of("\"")-(ValueToSet.find("\"")+1)));
    if(fOutputFolder.find_last_of('/')!=fOutputFolder.length()-1) {
      fOutputFolder.append("/");
    }
  } else if(WhatToSet.compare("RANDOM_SEED")==0) {
    fRandomSeed=std::stof(ValueToSet);
  } else if(WhatToSet.compare("ENTRANCE_CHANNEL")==0) {
    fEntranceChannel.assign(ValueToSet);
  } else if(WhatToSet.compare("EXIT_CHANNEL")==0) {
    fExitChannel.assign(ValueToSet);
  } else if(WhatToSet.compare("BEAM")==0) {
    do {
      if(ValueToSet.find("-Z")!=std::string::npos) {
        ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-Z=")+3)); 
        fZbeam=std::stoi(ValueToSet); 
      } else if(ValueToSet.find("-A")!=std::string::npos) {
        ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-A=")+3));
        fAbeam=std::stoi(ValueToSet); 
      }
    } while (LineStream>>ValueToSet);
  } else if(WhatToSet.compare("TARGET")==0) {
    do {
      if(ValueToSet.find("-Z")!=std::string::npos) {
        ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-Z=")+3)); 
        fZtarget=std::stoi(ValueToSet); 
      } else if(ValueToSet.find("-A")!=std::string::npos) {
        ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-A=")+3));
        fAtarget=std::stoi(ValueToSet); 
      }
    } while (LineStream>>ValueToSet);
  } else if(WhatToSet.compare("EJECTILE")==0) {
    do {
      if(ValueToSet.find("-Z")!=std::string::npos) {
        ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-Z=")+3)); 
        fZejectile=std::stoi(ValueToSet); 
      } else if(ValueToSet.find("-A")!=std::string::npos) {
        ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-A=")+3));
        fAejectile=std::stoi(ValueToSet); 
      }
    } while (LineStream>>ValueToSet);
  } else if(WhatToSet.compare("BEAM_ENERGY")==0) {
    fBeamEnergy=std::stof(ValueToSet);
  } else if(WhatToSet.compare("EX_RESIDUAL")==0) {
    fExresidual=std::stof(ValueToSet);
  } else if(WhatToSet.compare("GAS_MATERIAL")==0) {
    fGasMaterial.assign(ValueToSet);
  } else if(WhatToSet.compare("GAS_PRESSURE")==0) {
    fGasPressure=std::stof(ValueToSet);
  } else if(WhatToSet.compare("GAS_TEMPERATURE")==0) {
    fGasTemperature=std::stof(ValueToSet);
  } else if(WhatToSet.compare("WINDOW_MATERIAL")==0) {
    fWindowMaterial.assign(ValueToSet);
  } else if(WhatToSet.compare("WINDOW_THICKNESS")==0) {
    fWindowThickness=std::stof(ValueToSet);
  } else {
    return 0; 
  }
  
  return 1;
}

//____________________________________________________
int REXppFramework::ProcessDefineCommand(const char * line)
{
  std::string InputLine(line);
  std::istringstream LineStream(InputLine);  
  
  std::string Command;
  std::string WhatToSet;
  
  LineStream>>Command>>WhatToSet;
  
  if(WhatToSet.compare("SETUP")==0) {
    std::string SetupName;
    LineStream>>SetupName;
    SetupName.assign(SetupName.substr(SetupName.find("\"")+1,SetupName.find_last_of("\"")-(SetupName.find("\"")+1)));
    
    UNISDetectionSetup * NewSetup = new UNISDetectionSetup(SetupName.c_str());
    
    fExpSetup=NewSetup;
    
  } else {
    return 0; 
  }
  
  return 1;
}

//____________________________________________________
int REXppFramework::ProcessAddCommand(const char * line)
{
  std::string InputLine(line);
  std::istringstream LineStream(InputLine);
  
  std::string Command;
  std::string WhatToSet;
  
  LineStream>>Command>>WhatToSet;
  
  if(WhatToSet.compare("DETECTOR")==0) {
    //Adding a new detector
    std::string DetectorType;
    LineStream>>DetectorType;
    std::string ValueToSet;
            
    if(DetectorType.compare("STRIP")==0) { 
      double distance=0;
      double theta_pos=0;
      double phi_pos=0;
      int strip_number=0;
      double strip_width=0;
      double strip_inter=0;
      double frame_width=0;
      double dead_layer=0;
      bool is_double_sided=false;
      
      while (LineStream>>ValueToSet) {
        if(ValueToSet.find("-distance=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-distance=")+10)); 
          distance=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-theta=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-theta=")+7)); 
          theta_pos=std::stof(ValueToSet)*TMath::DegToRad();
        } else if(ValueToSet.find("-phi=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-phi=")+5)); 
          phi_pos=std::stof(ValueToSet)*TMath::DegToRad(); 
        } else if(ValueToSet.find("-strips=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-strips=")+8)); 
          strip_number=std::stoi(ValueToSet); 
        } else if(ValueToSet.find("-strip_width=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-strip_width=")+13)); 
          strip_width=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-inter_strip=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-inter_strip=")+13)); 
          strip_inter=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-frame_width=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-frame_width=")+13)); 
          frame_width=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-dead_layer=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-dead_layer=")+12)); 
          dead_layer=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-double_sided=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-double_sided=")+14)); 
          if(ValueToSet.find("yes")!=std::string::npos) is_double_sided=true;
        }
      }
      
      UNISDetectionUnit * NewDetector = 0;
      if(is_double_sided) NewDetector = new UNISStripDetector(distance,theta_pos,phi_pos,strip_number,strip_width,strip_inter,frame_width,dead_layer,"");
      else NewDetector = new UNISStripSingleSidedDetector(distance,theta_pos,phi_pos,strip_number,strip_width,strip_inter,frame_width,dead_layer,"");
      fExpSetup->RegisterUnit(NewDetector);
      
    } else if(DetectorType.compare("STRIP_ROT")==0) {
      double X0=0;
      double Y0=0;
      double Z0=0;
      double tilt_X=0;
      double tilt_Y=0;
      int strip_number=0;
      double strip_width=0;
      double strip_inter=0;
      double frame_width=0;
      double dead_layer=0;
      bool is_double_sided=false;
      
      while (LineStream>>ValueToSet) {
        if(ValueToSet.find("-X0=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-X0=")+4)); 
          X0=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-Y0=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-Y0=")+4)); 
          Y0=std::stof(ValueToSet);
        } else if(ValueToSet.find("-Z0=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-Z0=")+4)); 
          Z0=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-tilt_X=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-tilt_X=")+8)); 
          tilt_X=std::stof(ValueToSet)*TMath::DegToRad();
        } else if(ValueToSet.find("-tilt_Y=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-tilt_Y=")+8)); 
          tilt_Y=std::stof(ValueToSet)*TMath::DegToRad();
        } else if(ValueToSet.find("-strips=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-strips=")+8)); 
          strip_number=std::stoi(ValueToSet); 
        } else if(ValueToSet.find("-strip_width=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-strip_width=")+13)); 
          strip_width=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-inter_strip=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-inter_strip=")+13)); 
          strip_inter=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-frame_width=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-frame_width=")+13)); 
          frame_width=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-dead_layer=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-dead_layer=")+12)); 
          dead_layer=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-double_sided=")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-double_sided=")+14)); 
          if(ValueToSet.find("yes")!=std::string::npos) is_double_sided=true;
        }
      }
      
      UNISDetectionUnit * NewDetector = 0;
      if(is_double_sided) NewDetector = new UNISStripDetector(X0,Y0,Z0,tilt_X,tilt_Y,strip_number,strip_width,strip_inter,frame_width,dead_layer,"");
      else NewDetector = new UNISStripSingleSidedDetector(X0,Y0,Z0,tilt_X,tilt_Y,strip_number,strip_width,strip_inter,frame_width,dead_layer,"");
      fExpSetup->RegisterUnit(NewDetector);
      
    } else if(DetectorType.compare("LAMP_WEDGE")==0) {
      double distance=0;
      double phi_pos=0;
      double tilt=0;
      double bottom_frame_distance=0;
      int strip_number=0;
      double strip_width=0;
      double strip_inter=0;
      while (LineStream>>ValueToSet) {
        if(ValueToSet.find("-distance")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-distance=")+10)); 
          distance=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-phi_pos")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-phi_pos=")+9)); 
          phi_pos=std::stof(ValueToSet)*TMath::DegToRad(); 
        } else if(ValueToSet.find("-tilt")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-tilt=")+6)); 
          tilt=std::stof(ValueToSet)*TMath::DegToRad(); 
        } else if(ValueToSet.find("-frame_distance")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-frame_distance=")+16)); 
          bottom_frame_distance=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-strips")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-strips=")+8)); 
          strip_number=std::stoi(ValueToSet); 
        } else if(ValueToSet.find("-strip_width")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-strip_width=")+13)); 
          strip_width=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-inter_strip")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-inter_strip=")+13)); 
          strip_inter=std::stof(ValueToSet); 
        }
      }
      
      UNISLampWedgeDetector * NewDetector = new UNISLampWedgeDetector(distance,phi_pos,tilt,bottom_frame_distance,strip_number,strip_width,strip_inter);
      fExpSetup->RegisterUnit(NewDetector);
    } else if(DetectorType.compare("LAMP_WEDGE_MMM")==0) {
      double distance=0;
      double phi_pos=0;
      double tilt=0;
      double bottom_frame_distance=0;
      double strip_inter=0;
      while (LineStream>>ValueToSet) {
        if(ValueToSet.find("-distance")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-distance=")+10)); 
          distance=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-phi_pos")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-phi_pos=")+9)); 
          phi_pos=std::stof(ValueToSet)*TMath::DegToRad(); 
        } else if(ValueToSet.find("-tilt")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-tilt=")+6)); 
          tilt=std::stof(ValueToSet)*TMath::DegToRad(); 
        } else if(ValueToSet.find("-frame_distance")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-frame_distance=")+16)); 
          bottom_frame_distance=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-inter_strip")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-inter_strip=")+13)); 
          strip_inter=std::stof(ValueToSet); 
        }
      }
      
      UNISLampWedgeMMMDetector * NewDetector = new UNISLampWedgeMMMDetector(distance,phi_pos,tilt,bottom_frame_distance,strip_inter);
      fExpSetup->RegisterUnit(NewDetector);
    } else if(DetectorType.compare("FAZIA_BLOCK")==0) {
      double displacement=0;
      double theta_pos=0;
      double phi_pos=0;
      double pad_width=0;
      double frame_width=0;
      while (LineStream>>ValueToSet) {
        if(ValueToSet.find("-displacement")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-displacement=")+14));
          displacement=std::stoi(ValueToSet); 
        } else if(ValueToSet.find("-phi_pos")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-phi_pos=")+9)); 
          phi_pos=std::stof(ValueToSet)*TMath::DegToRad(); 
        } else if(ValueToSet.find("-theta_pos")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-theta_pos=")+11)); 
          theta_pos=std::stof(ValueToSet)*TMath::DegToRad(); 
        } else if(ValueToSet.find("-pad_width")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-pad_width=")+11)); 
          pad_width=std::stof(ValueToSet); 
        } else if(ValueToSet.find("-frame_width")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-frame_width=")+13)); 
          frame_width=std::stof(ValueToSet); 
        }
      }
      UNISFaziaBlock * NewDetector = new UNISFaziaBlock(theta_pos,phi_pos,displacement,pad_width,frame_width);
      fExpSetup->RegisterUnit(NewDetector);
    } else if(DetectorType.compare("PHOTO_DIODE")==0) {
      double distance=0;
      double theta_pos=0;
      double phi_pos=0;
      while (LineStream>>ValueToSet) {
        if(ValueToSet.find("-distance")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-distance=")+10));
          distance=std::stoi(ValueToSet); 
        } else if(ValueToSet.find("-phi_pos")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-phi_pos=")+9)); 
          phi_pos=std::stof(ValueToSet)*TMath::DegToRad(); 
        } else if(ValueToSet.find("-theta_pos")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-theta_pos=")+11)); 
          theta_pos=std::stof(ValueToSet)*TMath::DegToRad(); 
        } 
      }
      UNISSiliconPhotoDiode * NewDetector = new UNISSiliconPhotoDiode(distance,theta_pos,phi_pos);
      fExpSetup->RegisterUnit(NewDetector);
    } else if(DetectorType.compare("OSCAR")==0) {
      double distance=0;
      double theta_pos=0;
      double phi_pos=0;
      bool is_strip=true;
      bool is_collimator=false;
      while (LineStream>>ValueToSet) {
        if(ValueToSet.find("-distance")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-distance=")+10));
          distance=std::stoi(ValueToSet); 
        } else if(ValueToSet.find("-phi_pos")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-phi_pos=")+9)); 
          phi_pos=std::stof(ValueToSet)*TMath::DegToRad(); 
        } else if(ValueToSet.find("-theta_pos")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-theta_pos=")+11)); 
          theta_pos=std::stof(ValueToSet)*TMath::DegToRad(); 
        } else if(ValueToSet.find("-strip")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-strip=")+7)); 
          if(ValueToSet.find("no")!=std::string::npos) is_strip=false; 
        } else if(ValueToSet.find("-collimator")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-collimator=")+7)); 
          if(ValueToSet.find("yes")!=std::string::npos) is_collimator=true;
        } 
      }
      UNISOscarTelescope * NewDetector = new UNISOscarTelescope(distance,theta_pos,phi_pos,(is_strip && !is_collimator ? "" : (is_strip && is_collimator ? "col" : (is_collimator ? "pads col" : "pads"))));
      fExpSetup->RegisterUnit(NewDetector);
    } else if(DetectorType.compare("COLLIMATED_SILICON")==0) {
      double distance=0;
      double theta_pos=0;
      double phi_pos=0;
      double collimator_inner_radius=0;
      double collimator_outer_radius=0;
      while (LineStream>>ValueToSet) {
        if(ValueToSet.find("-distance")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-distance=")+10));
          distance=std::stoi(ValueToSet); 
        } else if(ValueToSet.find("-phi_pos")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-phi_pos=")+9)); 
          phi_pos=std::stof(ValueToSet)*TMath::DegToRad(); 
        } else if(ValueToSet.find("-theta_pos")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-theta_pos=")+11)); 
          theta_pos=std::stof(ValueToSet)*TMath::DegToRad(); 
        } else if(ValueToSet.find("-inner_radius")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-inner_radius=")+14)); 
          collimator_inner_radius=std::stof(ValueToSet);
        } else if(ValueToSet.find("-outer_radius")!=std::string::npos) {
          ValueToSet.assign(ValueToSet.substr(ValueToSet.find("-outer_radius=")+14)); 
          collimator_outer_radius=std::stof(ValueToSet);
        } 
      }
      UNISCollimatedSilicon * NewDetector = new UNISCollimatedSilicon(distance,theta_pos,phi_pos,collimator_inner_radius,collimator_outer_radius);
      fExpSetup->RegisterUnit(NewDetector);
    }
    
  } else {
    return 0; 
  }
  
  return 1;
}

//____________________________________________________
void REXppFramework::PrintConfiguration() const
{
  printf("\nThe REX++ Framework\n");
  
  //
  //Printing date and time
  time_t now;
  time(&now);
  printf("Date and time: %s\n", ctime(&now));
  //
  
  //
  //Printing configuration quantities
  printf("----------------------\n");
  printf("Calculation Step = %.3f mm\n", fCalculationStep*10);
  printf("Energy loss precision = %e\n", fEnergyLossPrecision);
  printf("Verbose Mode: %s\n", fVerbose ? "on" : "off");
  printf("Graphical Mode: %s\n", fGraphics&&!fAdvancedGraphics ? "on (light)" : (fGraphics&&fAdvancedGraphics ? "on (advanced)" : "off"));
  printf("----------------------\n");
  printf("Entrance Channel: %s\n", fEntranceChannel.c_str());
  printf("Exit Channel: %s\n", fExitChannel.c_str());
  printf("Beam: Z=%d A=%d\n", fZbeam, fAbeam);
  printf("Target: Z=%d A=%d\n", fZtarget, fAtarget);
  printf("Ejectile: Z=%d A=%d\n", fZejectile, fAejectile);
  printf("Ex residual = %.2f MeV\n", fExresidual);
  printf("----------------------\n");
  printf("Beam Energy = %.2f MeV\n", fBeamEnergy);
  printf("Entrance Window: %s(%.2f um)\n", fWindowMaterial.c_str(), fWindowThickness);
  printf("Gas Material: %s(%.0f mbar, T=%.2fK)\n", fGasMaterial.c_str(), fGasPressure, fGasTemperature);
  printf("----------------------\n");
  printf("Detection Setup: \"%s\" (%d detectors)\n", fExpSetup->GetName(), fExpSetup->Size());
  printf("----------------------\n");
  //
  
  printf("\n");
}

//____________________________________________________
void REXppFramework::InitTree()
{
  //
  //Creating output file
  fOutputFileName.assign(Form("REXpp_%s_%s_%s%.2fum_%.0fmbar%.2fK_%.0fMeV_Ex%.2f_%devents.root",fEntranceChannel.c_str(),fExitChannel.c_str(),fWindowMaterial.c_str(),fWindowThickness,fGasPressure,fGasTemperature,fBeamEnergy,fCompoundEx,fNumEvents));
  TheFile = new TFile(Form("%s%s",fOutputFolder.c_str(),fOutputFileName.c_str()),"RECREATE");
  //
  
  //
  //Allocating output tree
  TheTree = new TTree("t", "REXpp Framework Data");
  //
  
  //
  //Allocating data structure
  fevt = new REXppRootEvent();
  //
  
  //
  //Creating Thee branch  
  TheTree->Branch(Form("%s.", fExpSetup->GetName()), "REXppRootEvent", fevt, 3200, 2);
  //
  
  //
  //Setting Autosave
  TheTree->SetAutoSave(500000);
  //
}

//____________________________________________________
void REXppFramework::EndProcess()
{
  //
  //Saving tree to file
  TheTree->AutoSave();
  //
  
  //
  //Closing Root File
  TheFile->Close();
  //
  
  //
  printf("Successfully closed file %s\n", Form("%s%s",fOutputFolder.c_str(),fOutputFileName.c_str()));
  //
}

//____________________________________________________
int REXppFramework::ReadInput(int argc, char ** argv)
{
  int read=0;
  for(int i=1; i<argc; i++) {
    if(strcmp(argv[i],"-events")==0) {
      SetIterations(atoi(argv[++i]));
      read++;
    } else if(strcmp(argv[i],"-Ex")==0) {
      SetEx(atof(argv[++i]));
      read++;
    }
  }
  
  return read;
}

//____________________________________________________
void REXppFramework::ProcessIterations()
{
  //This is the core method.
  //Here we process, event-by-event, the framework iterations.
  //The tree is filled for each event.
  
  fStartTime=clock();
  
  const double BeamMomentumModuleInit=sqrt(pow(fBeamEnergy+fMassBeam,2)-pow(fMassBeam,2)); //Initial module of the beam momentum
  const double ParticleEmissionThreshold=fMassEjectile+fMassResidual-fMassCompound_gs;
  const TVector3 BeamMomentumInit=BeamMomentumModuleInit*TVector3(0,0,1);
  const TLorentzVector BeamMomentumEnergyInit(BeamMomentumInit,fBeamEnergy+fMassBeam); //Initial beam Momentum/Energy quadri-vector
  const TLorentzVector TargetMomentumEnergy(0.,0.,0.,fMassTarget); // Target Momentum/Energy quadri-vector
  double ExAvailable = (BeamMomentumEnergyInit+TargetMomentumEnergy).M()-fMassCompound_gs; // Excitation energy available in the collision
  if(ExAvailable<fCompoundEx) {
    printf("Error: insufficient beam energy to reach the requested excitation energy, Aborting!\n");
    exit(2);
  }
  if(fCompoundEx<ParticleEmissionThreshold) {
    printf("Error: the selected excitation energy is below the particle-emission threshold, Aborting!\n");
    exit(3);
  }  
  //
  TVector3 XVersor(0,1,0); //Versor identifying the X-axis in the laboratory frame (pointing towards the top)
  TVector3 YVersor(1,0,0); //Versor identifying the Y-axis in the laboratory frame (pointing towards the right if looking upstream) 
  TVector3 ZVersor(0,0,1); //Versor identifying the beam axis in the laboratory frame
  //
  //Angular distribution function
  TF1 * Mg24_ang_uniform = new TF1 ("Mg24_ang_uniform","sin(x)",0,TMath::Pi()); //uniform angular distribution for the compound nucleus decay in the cm
  //
  
  //
  if(fGraphics) {    
    //
    //Drawing mother volume
    if(!gGeoManager) {
      new TGeoManager("REXpp", "GL viewer for REXpp");
    }
    TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
    matVacuum->SetTransparency(60);
    TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
    TGeoVolume *TheMotherVolume = gGeoManager->MakeBox("TheMotherVolume", Vacuum, fLimitY, fLimitX, fLimitZ/2.);
    TheMotherVolume->SetLineColor(kGray);
//     gGeoManager->SetTopVisible(kFALSE); // the TOP is invisible
    gGeoManager->SetTopVolume(TheMotherVolume);
    TGeoHMatrix * mother_volume_matrix = new TGeoHMatrix("mother_volume_matrix");
    mother_volume_matrix->MultiplyLeft(new TGeoTranslation(0., 0., fLimitZ/2.));
    gGeoManager->GetMasterVolume()->AddNode(TheMotherVolume,1,mother_volume_matrix);
    //     TEveBox * MotherVolume = new TEveBox("MotherVolume");
    //     MotherVolume->SetVertex(0,-fLimitY,fLimitX,0);
    //     MotherVolume->SetVertex(1,fLimitY,fLimitX,0);
    //     MotherVolume->SetVertex(2,fLimitY,-fLimitX,0);
    //     MotherVolume->SetVertex(3,-fLimitY,-fLimitX,0);
    //     MotherVolume->SetVertex(4,-fLimitY,fLimitX,fLimitZ);
    //     MotherVolume->SetVertex(5,fLimitY,fLimitX,fLimitZ);
    //     MotherVolume->SetVertex(6,fLimitY,-fLimitX,fLimitZ);
    //     MotherVolume->SetVertex(7,-fLimitY,-fLimitX,fLimitZ);
    //     MotherVolume->SetMainColor(kCyan);
    //     MotherVolume->SetMainTransparency(99);
    //     gEve->AddElement(MotherVolume);
    //     gEve->DoRedraw3D();
    //
//     gGeoManager->CloseGeometry();
//     gGeoManager->SetVisLevel(4);
//     gGeoManager->SetVisOption(0);
    gStyle->SetCanvasPreferGL(kTRUE);
    fExpSetup->Draw3D(fAdvancedGraphics ? "ogl" : "gl");
  }
  //
  
  //
  if(fVerbose) {
    printf("*** Framework REX++ correctly created\n");
    printf("Projectile: Z=%d A=%d (mass=%.3f MeV/c2)\n", fZbeam, fAbeam, fMassBeam);
    printf("Target: Z=%d A=%d (mass=%.3f MeV/c2)\n", fZtarget, fAtarget, fMassTarget);
    printf("Ejectile: Z=%d A=%d (mass=%.3f MeV/c2)\n", fZejectile, fAejectile, fMassEjectile);
    printf("Residual: Z=%d A=%d (mass=%.3f MeV/c2, Ex=%.2f)\n", fZresidual, fAresidual, fMassResidual, fExresidual);
    printf("Ebeam=%.2f MeV\n", fBeamEnergy);
    printf("Calculation step = %.1f mm\n", fCalculationStep*10);
    printf("Gas material = %s (%.0f mbar)\n", fGasMaterial.c_str(), fGasPressure);
    printf("**************************************************\n\n"); 
  }
  //
  
  //
  //Main Iteration
  for(int ientry=0; ientry<fNumEvents; ientry++)
  {
    //
    //The following framework executes a step-by-step calculation with a fixed step
    //For each step, three quantities are calculated: (1) the energy loss in the step, (2) the energy straggling, (3) the angular straggling
    //After interaction, the 2 fragments are tracked till one of the follwing events occur: (1) the particle is stopped in the gas, (2) the particle reaches a detector, (3) the particle reaches the limit of the useful volume
    //
    
    if(ientry%10000==0) {
      PrintPercentage(ientry, fNumEvents);
    }
    
    if(fVerbose) {
      printf("*** Beginning of iteration %d\n", ientry);
      printf("**************************************************\n\n");
    }
    
    //Initial beam energy, position and direction of the beam
    double EffectiveBeamEnergy=fBeamEnergy; //NOTE: the effective beam energy can be different with respect to the nominal one  
    
    //Pre and Post step quantities of the beam
    //
    //Position of the beam before the entrance window
    TVector3 PreStepBeamLocation(fBeamCenterY,fBeamCenterX,-fWindowThickness/fcmToum); //Absolute position of the beam with respect to the exit face of the entrance window in cm (Entrance Window reference frame)
    //
    //Momentum of the beam prior to interact with the entrance window
    double PreStepBeamMomentumModule=sqrt(pow(EffectiveBeamEnergy+fMassBeam,2)-pow(fMassBeam,2));
    TVector3 PreStepBeamMomentum(PreStepBeamMomentumModule*ZVersor);
    TVector3 PreStepBeamDirection(PreStepBeamMomentum.Unit());
    double PreStepBeamKineticEnergy=EffectiveBeamEnergy;
    TLorentzVector PreStepBeamMomentumEnergy(PreStepBeamMomentum,PreStepBeamKineticEnergy+fMassBeam);
    //
    double PostStepBeamMomentumModule;
    TVector3 PostStepBeamMomentum;
    TVector3 PostStepBeamDirection;
    TVector3 PostStepBeamLocation;
    double PostStepBeamKineticEnergy;
    TLorentzVector PostStepBeamMomentumEnergy;
    //
    
    //
    //Interaction of the beam with the entrance window
    SteppingActionWindow(PreStepBeamMomentumEnergy,PreStepBeamLocation,PostStepBeamMomentumEnergy,PostStepBeamLocation);
    //
    TLorentzVector BeamMomentumEnergyAfterWindow=PostStepBeamMomentumEnergy;
    PostStepBeamMomentumModule=BeamMomentumEnergyAfterWindow.P();
    PostStepBeamMomentum=BeamMomentumEnergyAfterWindow.Vect();
    PostStepBeamDirection=PostStepBeamMomentum.Unit();
    PostStepBeamKineticEnergy=BeamMomentumEnergyAfterWindow.E()-BeamMomentumEnergyAfterWindow.M();
    //
    //After the interaction of the beam with the entrance window I have the following quantities:
    //PostStepBeamMomentum = The beam momentum after the last step as a TVector3
    //PostStepBeamMomentumModule = Its module after the last step
    //PreStepBeamMomentum = The beam momentum before the last step as a TVector3
    //PreStepBeamMomentumModule = Its module before the last step
    //PostStepBeamLocation = The current beam position as a TVector3
    //PostStepBeamDirection = Direction of the beam after the last step as a TVector3
    //PreStepBeamDirection = Direction of the beam before the last step as a TVector3
    //PostStepBeamKineticEnergy = Energy of the beam after the last step of calculation
    //BeamMomentumEnergyAfterWindow = quadri-vector representing the beam after the entrace window
    //
    //WARNING: TEMPORARLY TRACKS ARE NOT DRAWN
    //     if(fGraphics) {
    //       TrackBeam = new TEveStraightLineSet("TrackBeam");
    //       TrackBeam->SetMainColor(kWhite);
    //     }
    
    ExAvailable=(BeamMomentumEnergyAfterWindow+TargetMomentumEnergy).M()-fMassCompound_gs; // Excitation energy available in the collision
    if(ExAvailable<fCompoundEx) {
      printf("Error: insufficient beam energy to reach the requested excitation energy, Aborting!\n");
      exit(2);
    }
    
    //
    //Iterations till the Ex is reached
    //
    double BeamDegradationStep=fCalculationStep;
    //
    //Setting Pre-Step to the previous Post-Step
    PreStepBeamMomentumModule=PostStepBeamMomentumModule;
    PreStepBeamMomentum=PostStepBeamMomentum;
    PreStepBeamDirection=PostStepBeamDirection;
    PreStepBeamLocation=PostStepBeamLocation;
    PreStepBeamKineticEnergy=PostStepBeamKineticEnergy;
    PreStepBeamMomentumEnergy=PostStepBeamMomentumEnergy;
    //
    while(1) {
      
      //
      //Check if the beam is inside the mother volume
      if(fabs(PreStepBeamLocation.X())>fLimitX || fabs(PreStepBeamLocation.Y())>fLimitY || fabs(PreStepBeamLocation.Z())>fLimitZ) {
        printf("Error: beam escaped from the mother volume without reaching the required Ex, aborting!\n");
        exit(1);
      }
      //
      
      //
      //Energy degradation step
      SteppingActionGas(PreStepBeamMomentumEnergy,PreStepBeamLocation,PostStepBeamMomentumEnergy,PostStepBeamLocation,fZbeam,fAbeam,BeamDegradationStep);
      //
      PostStepBeamMomentumModule=PostStepBeamMomentumEnergy.P();
      PostStepBeamMomentum=PostStepBeamMomentumEnergy.Vect();
      PostStepBeamDirection=PostStepBeamMomentum.Unit();
      PostStepBeamKineticEnergy=PostStepBeamMomentumEnergy.E()-PostStepBeamMomentumEnergy.M();
      //
      double CurrCompoundEx=(PostStepBeamMomentumEnergy+TargetMomentumEnergy).M()-fMassCompound_gs;
      if(fVerbose) {
        printf("**\n");
        printf("Ex(Compound): %.3f MeV\n", CurrCompoundEx);
        printf("**************************************************\n\n");
      }
      //
      
      //Check if Ex is reached
      //Ex is reached within 1 keV of precision
      //If the current excitation energy is lower than the required excitation energy -> 
      if(fabs(CurrCompoundEx-fCompoundEx)<0.001) {
        //Reached the required excitation energy -> formation of the compound nucleus
        if(fVerbose) {
          printf("*** Excitation energy reached Ex=%.2f\n", CurrCompoundEx);
          printf("**************************************************\n\n");
          getchar();
        }
        fEffectiveCompoundEx=CurrCompoundEx;
        break;
      } else if (CurrCompoundEx<fCompoundEx) {
        //The required excitation energy has been passed, undo last step and re-do it with half of its length
        BeamDegradationStep/=2.;
        continue;
      }
      
      //The required excitation energy is not reached, I go to the next step
      //To conclde the step -> Setting Pre-Step to the previous Post-Step
      //Setting Pre-Step to the previous Post-Step
      PreStepBeamMomentumModule=PostStepBeamMomentumModule;
      PreStepBeamMomentum=PostStepBeamMomentum;
      PreStepBeamDirection=PostStepBeamDirection;
      PreStepBeamLocation=PostStepBeamLocation;
      PreStepBeamKineticEnergy=PostStepBeamKineticEnergy;
      PreStepBeamMomentumEnergy=PostStepBeamMomentumEnergy;
      //
      //WARNING: TEMPORARLY TRACKS ARE NOT DRAWN
      //       if(fGraphics) {
      //         TrackBeam->AddLine(-PreStepBeamLocation.Y(),PreStepBeamLocation.X(),PreStepBeamLocation.Z(),-PostStepBeamLocation.Y(),PostStepBeamLocation.X(),PostStepBeamLocation.Z());     
      //       }
      //
    }
    //
    //End of Ex iteration
    
    //The effective beam energy reached at the formation of the compound
    EffectiveBeamEnergy=PostStepBeamKineticEnergy;
    
    //The final beam location when Ex is reached
    TVector3 BeamLocation(PostStepBeamLocation);
    
    //The beam as a quadri-vector when Ex is reached
    TLorentzVector BeamMomentumEnergy(PostStepBeamMomentum,EffectiveBeamEnergy+fMassBeam);
    
    //
    //Formation and decay of the compound nucleus
    TLorentzVector CompoundMomentumEnergy=BeamMomentumEnergy+TargetMomentumEnergy;
    const double InvariantMass=CompoundMomentumEnergy.M(); //this should be a constant as the Ex is fixed. However we might have small fluctuations arising from the algorithm of Ex (=position in the gas) determination
    const double cm_angle=Mg24_ang_uniform->GetRandom(0,TMath::Pi()); //Emission angle in the center of mass of the decay
    const double cm_angle_phi=fRandomGen->Uniform(0,2*TMath::Pi()); //Emission phi angle in the center of mass of the decay
    const double MomentumCmModule=sqrt((pow(InvariantMass,4)+pow((pow(fMassEjectile,2)-pow(fMassResidual,2)),2)-2*pow(InvariantMass,2)*(pow(fMassEjectile,2)+pow(fMassResidual,2)))/(4*pow(InvariantMass,2))); //Module of the momentum in the cm
    TVector3 EjectileMomentumCm(MomentumCmModule*sin(cm_angle)*cos(cm_angle_phi),MomentumCmModule*sin(cm_angle)*sin(cm_angle_phi),MomentumCmModule*cos(cm_angle)); //Momentum of the ejectile in the cm
    TLorentzVector EjectileMomentumEnergy(EjectileMomentumCm,sqrt(pow(MomentumCmModule,2)+pow(fMassEjectile,2))); //Quadri-Momentum of the ejectile in the cm
    TLorentzVector ResidualMomentumEnergy(-EjectileMomentumCm,sqrt(pow(MomentumCmModule,2)+pow(fMassResidual,2))); //Quadri-Momentum of the residual in the cm
    //Saving quantities in the cms
    const double EjectileKinEnergyCms=EjectileMomentumEnergy.E()-fMassEjectile;
    const double ResidualKinEnergyCms=ResidualMomentumEnergy.E()-fMassResidual;
    const double EjectileThetaCms=EjectileMomentumEnergy.Theta();
    const double ResidualThetaCms=ResidualMomentumEnergy.Theta();
    //Conversion of quantities in the lab frame
    EjectileMomentumEnergy.Boost(CompoundMomentumEnergy.BoostVector()); //Boosting the ejectile cm momentum in the lab frame
    ResidualMomentumEnergy.Boost(CompoundMomentumEnergy.BoostVector()); //Boosting the residual cm momentum in the lab frame
    TVector3 EjectileLocation(BeamLocation);
    TVector3 ResidualLocation(BeamLocation);
    
    //WARNING: TEMPORARLY TRACKS ARE NOT DRAWN
    //     if(fGraphics) {    
    //       gEve->AddElement(TrackBeam);
    //       gEve->Redraw3D();
    //     }    
    
    if(fVerbose) {
      printf("*** Decay of the compound nucleus\n");
      printf("Theta_cm: %.2f deg\n", cm_angle*TMath::RadToDeg());
      printf("Ejectile theta: %.2f deg\n", EjectileMomentumEnergy.Theta()*TMath::RadToDeg());
      printf("Ejectile Ekin: %.2f MeV\n", EjectileMomentumEnergy.E()-fMassEjectile);
      printf("Residual theta: %.2f deg\n", ResidualMomentumEnergy.Theta()*TMath::RadToDeg());
      printf("Residual Ekin: %.2f MeV\n", ResidualMomentumEnergy.E()-fMassResidual);
      printf("**************************************************\n\n");
      getchar();
    }
    
    //
    //Detection process
    //The process terminates with: 
    // (1) detection of a fragment 
    // (2) the fragment escapes from the mother volume
    // (3) the fragment stops in the gas
    //The process is always binary.
    //
    fevt->fEx=fCompoundEx; //Excitation energy of the compound
    fevt->fExEffective=fEffectiveCompoundEx; //Excitation energy of the compound as reached in the simulation
    //
    //Ejectile
    //
    fevt->fmulti=0;
    fevt->fmulti_detected=0;
    fevt->fIsDetected[fevt->fmulti]=false;
    fevt->fIsStopped[fevt->fmulti]=false;
    fevt->fnumdet[fevt->fmulti]=-1;
    fevt->fnumpixel[fevt->fmulti]=-1;
    fevt->fZ[fevt->fmulti]=fZejectile;
    fevt->fA[fevt->fmulti]=fAejectile;
    fevt->fKinEnergyOrigin[fevt->fmulti]=EjectileMomentumEnergy.E()-fMassEjectile;
    fevt->fThetaOrigin[fevt->fmulti]=EjectileMomentumEnergy.Theta();
    fevt->fPhiOrigin[fevt->fmulti]=EjectileMomentumEnergy.Phi();
    fevt->fKinEnergyOriginCms[fevt->fmulti]=EjectileKinEnergyCms;
    fevt->fThetaOriginCms[fevt->fmulti]=EjectileThetaCms;
    fevt->fKinEnergy[fevt->fmulti]=-9999;
    fevt->fXDetHit[fevt->fmulti]=-9999;
    fevt->fYDetHit[fevt->fmulti]=-9999;
    fevt->fZDetHit[fevt->fmulti]=-9999;
    //
    //Pre and Post step quantities
    double EjectileDetectionStep=fCalculationStep;
    //
    double PreStepEjectileMomentumModule=EjectileMomentumEnergy.P();
    TVector3 PreStepEjectileMomentum(EjectileMomentumEnergy.Vect());
    TVector3 PreStepEjectileDirection(PreStepEjectileMomentum.Unit());
    TVector3 PreStepEjectileLocation(EjectileLocation);
    double PreStepEjectileKineticEnergy=EjectileMomentumEnergy.E()-fMassEjectile;
    TLorentzVector PreStepEjectileMomentumEnergy(PreStepEjectileMomentum,PreStepEjectileKineticEnergy+fMassEjectile);
    
    //
    double PostStepEjectileMomentumModule;
    TVector3 PostStepEjectileMomentum;
    TVector3 PostStepEjectileDirection;
    TVector3 PostStepEjectileLocation;
    double PostStepEjectileKineticEnergy;
    TLorentzVector PostStepEjectileMomentumEnergy;
    //
    //WARNING: TEMPORARLY TRACKS ARE NOT DRAWN
    //     if(fGraphics) {
    //       TrackEjectile = new TEveStraightLineSet("TrackEjectile");
    //       TrackEjectile->SetMainColor(kGreen+1);
    //     }
    //Main iteration for the detection process
    while(1) {
      //Check if the particle is inside the mother volume
      if(fabs(PreStepEjectileLocation.X())>fLimitX || fabs(PreStepEjectileLocation.Y())>fLimitY || fabs(PreStepEjectileLocation.Z())>fLimitZ || PreStepEjectileLocation.Z()<0) {
        //The particle is not detected --> all detector numbers are -1 but the "Origin" quantities are set anyway
        if(fVerbose) {
          printf("*** Particle escaped from the mother volume!\n"); 
          printf("**************************************************\n\n");
          getchar();
        }
        break;
      }
      
      //
      //Energy degradation step
      SteppingActionGas(PreStepEjectileMomentumEnergy,PreStepEjectileLocation,PostStepEjectileMomentumEnergy,PostStepEjectileLocation,fZejectile,fAejectile,EjectileDetectionStep);
      //
      PostStepEjectileMomentumModule=PostStepEjectileMomentumEnergy.P();
      PostStepEjectileMomentum=PostStepEjectileMomentumEnergy.Vect();
      PostStepEjectileDirection=PostStepEjectileMomentum.Unit();
      PostStepEjectileKineticEnergy=PostStepEjectileMomentumEnergy.E()-PostStepEjectileMomentumEnergy.M();
      //
      
      //
      //Check if the particle stopped in the gas
      if(PostStepEjectileKineticEnergy<1E-7) {
        //The particle stopped in the gas --> all detector numbers are -1 but the "Origin" quantities are set anyway
        fevt->fIsStopped[fevt->fmulti]=true;
        if(fVerbose) {
          printf("*** Particle stopped in the gas volume!\n"); 
          printf("**************************************************\n\n");
          getchar();
        }
        break;
      }
      //
      
      //
      //Check if the particle is detected
      //We check if the particle angle is inside the coverage of the detector
      //If the particle is not inside the coverage, the iteration continues and a successive step is calculated
      //If the particle is inside the coverage, we evaluate the distance between the particle location and the hit point on the surface of the detector
      //If such distance is lower than 1/10 of the calculation step, the particle is detected, if is larger the iteration continues and a successive step is calculated
      //If we pass the detector without reaching a distance lower than 1/10 of the calculation step, the iteration is repeated with half of the previous step.
      if(PreStepEjectileKineticEnergy>=fDetectorThreshold && fExpSetup->IsInside(PostStepEjectileDirection.Theta(),PostStepEjectileDirection.Phi(),PreStepEjectileLocation.X(),PreStepEjectileLocation.Y(),PreStepEjectileLocation.Z())) 
      {
        //The particle identifies a direction within the detector coverage
        //
        //Getting the detector whose direction is identified by the particle
        int TheDetectorIndex = fExpSetup->GetDetectorIndex(PostStepEjectileDirection.Theta(),PostStepEjectileDirection.Phi(),PreStepEjectileLocation.X(),PreStepEjectileLocation.Y(),PreStepEjectileLocation.Z());
        UNISDetectionUnit * TheDetector = fExpSetup->GetDetector(TheDetectorIndex);
        //Calculation of the distance between the potential hit point and the particle
        TVector3 HitPoint(TheDetector->GetImpactPointLab(PostStepEjectileDirection.Theta(),PostStepEjectileDirection.Phi(),PreStepEjectileLocation.X(),PreStepEjectileLocation.Y(),PreStepEjectileLocation.Z()));
        TVector3 HitPointRelative = HitPoint-PostStepEjectileLocation; //vector identifying the hit point with respect to the post step location
        double HitPointDistance=HitPointRelative.Dot(PostStepEjectileDirection); //Projection of the distance vector along the particle direction
        //
        //Evaluation of the distance
        if(fabs(HitPointDistance)<0.1*fCalculationStep) {
          //The particle is at the surface of the detetector
          if(fVerbose) {
            printf("*** Particle detected!\n"); 
            printf("**************************************************\n\n");
            getchar();
          }
          fevt->fnumdet[fevt->fmulti]=TheDetectorIndex;
          fevt->fnumpixel[fevt->fmulti]=TheDetector->GetPixel(PostStepEjectileDirection.Theta(),PostStepEjectileDirection.Phi(),PreStepEjectileLocation.X(),PreStepEjectileLocation.Y(),PreStepEjectileLocation.Z());
          //
          //Check if interstrip
          if(fevt->fnumpixel[fevt->fmulti]>=0) {   
            //Not interstrip event  
            fevt->fmulti_detected++;
            fevt->fIsDetected[fevt->fmulti]=true;
            fevt->fKinEnergy[fevt->fmulti]=PostStepEjectileKineticEnergy+ (fDetectorEres>0 ? fRandomGen->Gaus(0,PostStepEjectileKineticEnergy*fDetectorEres/2.355) : 0);
            fevt->fXDetHit[fevt->fmulti]=HitPoint.X();
            fevt->fYDetHit[fevt->fmulti]=HitPoint.Y();
            fevt->fZDetHit[fevt->fmulti]=HitPoint.Z();
          }
          //
          break;
        }
        //
        //Check if particle passed the detector without being detected
        if(HitPointDistance<0) {
          //The particle crossed the detector, we undo the last step and perform a step of 1/2 length
          EjectileDetectionStep/=2.;
          //
          //Undo the last step
          continue;
        }
      }
      //
      
      //The particle is not inside or the direction or is inside but the distance is greater than 1/10, I process the next step by using the standard calculation step
      EjectileDetectionStep=fCalculationStep;
      //Finalizing the step -> setting the pre step quantities to the current post step ones
      //WARNING: TEMPORARLY TRACKS ARE NOT DRAWN
      //       if(fGraphics) {
      //         TrackEjectile->AddLine(-PreStepEjectileLocation.Y(),PreStepEjectileLocation.X(),PreStepEjectileLocation.Z(),-PostStepEjectileLocation.Y(),PostStepEjectileLocation.X(),PostStepEjectileLocation.Z());
      //       }
      PreStepEjectileKineticEnergy=PostStepEjectileKineticEnergy;
      PreStepEjectileMomentumModule=PostStepEjectileMomentumModule;
      PreStepEjectileMomentum=PostStepEjectileMomentum;
      PreStepEjectileDirection=PostStepEjectileDirection;
      PreStepEjectileLocation=PostStepEjectileLocation;
      PreStepEjectileMomentumEnergy=PostStepEjectileMomentumEnergy;
    } //End of the main iteration for the detection process
    //WARNING: TEMPORARLY TRACKS ARE NOT DRAWN
    //     if(fGraphics) {
    //       gEve->AddElement(TrackEjectile);
    //       gEve->Redraw3D();
    //     }
    //
    
    fevt->fmulti++; //NOTE: in the current version of the program, the multiplicity is always equal to the number of particles detected
    
    //
    //Residual
    //
    fevt->fIsDetected[fevt->fmulti]=false;
    fevt->fIsStopped[fevt->fmulti]=false;
    fevt->fnumdet[fevt->fmulti]=-1;
    fevt->fnumpixel[fevt->fmulti]=-1;
    fevt->fZ[fevt->fmulti]=fZresidual;
    fevt->fA[fevt->fmulti]=fAresidual;
    fevt->fKinEnergyOrigin[fevt->fmulti]=ResidualMomentumEnergy.E()-fMassResidual;
    fevt->fThetaOrigin[fevt->fmulti]=ResidualMomentumEnergy.Theta();
    fevt->fPhiOrigin[fevt->fmulti]=ResidualMomentumEnergy.Phi();
    fevt->fKinEnergyOriginCms[fevt->fmulti]=ResidualKinEnergyCms;
    fevt->fThetaOriginCms[fevt->fmulti]=ResidualThetaCms;    
    fevt->fKinEnergy[fevt->fmulti]=-9999;
    fevt->fXDetHit[fevt->fmulti]=-9999;
    fevt->fYDetHit[fevt->fmulti]=-9999;
    fevt->fZDetHit[fevt->fmulti]=-9999;
    //
    //Pre and Post step quantities
    double ResidualDetectionStep=fCalculationStep;
    //
    double PreStepResidualMomentumModule=ResidualMomentumEnergy.P();
    TVector3 PreStepResidualMomentum(ResidualMomentumEnergy.Vect());
    TVector3 PreStepResidualDirection(PreStepResidualMomentum.Unit());
    TVector3 PreStepResidualLocation(ResidualLocation);
    double PreStepResidualKineticEnergy=ResidualMomentumEnergy.E()-fMassResidual;
    TLorentzVector PreStepResidualMomentumEnergy(PreStepResidualMomentum,PreStepResidualKineticEnergy+fMassResidual);
    
    //
    double PostStepResidualMomentumModule;
    TVector3 PostStepResidualMomentum;
    TVector3 PostStepResidualDirection;
    TVector3 PostStepResidualLocation;
    double PostStepResidualKineticEnergy;
    TLorentzVector PostStepResidualMomentumEnergy;
    //
    //
    //WARNING: TEMPORARLY TRACKS ARE NOT DRAWN
    //     if(fGraphics) {
    //       TrackResidual = new TEveStraightLineSet("TrackResidual");
    //       TrackResidual->SetMainColor(kMagenta);
    //     }
    //Main iteration for the detection process
    while(1) {
      //Check if the particle is inside the mother volume
      if(fabs(PreStepResidualLocation.X())>fLimitX || fabs(PreStepResidualLocation.Y())>fLimitY || fabs(PreStepResidualLocation.Z())>fLimitZ || PreStepResidualLocation.Z()<0) {
        //The particle is not detected --> all detector numbers are -1 but the "Origin" quantities are set anyway
        if(fVerbose) {
          printf("*** Particle escaped from the mother volume!\n"); 
          printf("**************************************************\n\n");
          getchar();
        }
        break;
      }
      
      //
      //Energy degradation step
      SteppingActionGas(PreStepResidualMomentumEnergy,PreStepResidualLocation,PostStepResidualMomentumEnergy,PostStepResidualLocation,fZresidual,fAresidual,ResidualDetectionStep);
      //
      PostStepResidualMomentumModule=PostStepResidualMomentumEnergy.P();
      PostStepResidualMomentum=PostStepResidualMomentumEnergy.Vect();
      PostStepResidualDirection=PostStepResidualMomentum.Unit();
      PostStepResidualKineticEnergy=PostStepResidualMomentumEnergy.E()-PostStepResidualMomentumEnergy.M();
      //
      
      //
      //Check if the particle stopped in the gas
      if(PostStepResidualKineticEnergy<1E-7) {
        fevt->fIsStopped[fevt->fmulti]=true;
        //The particle stopped in the gas --> all detector numbers are -1 but the "Origin" quantities are set anyway
        if(fVerbose) {
          printf("*** Particle stopped in the gas volume!\n"); 
          printf("**************************************************\n\n");
          getchar();
        }
        break;
      }
      //
      
      //
      //Check if the particle is detected
      //We check if the particle angle is inside the coverage of the detector
      //If the particle is not inside the coverage, the iteration continues and a successive step is calculated
      //If the particle is inside the coverage, we evaluate the distance between the particle location and the hit point on the surface of the detector
      //If such distance is lower than 1/10 of the calculation step, the particle is detected, if is larger the iteration continues and a successive step is calculated
      //If we pass the detector without reaching a distance lower than 1/10 of the calculation step, the iteration is repeated with half of the previous step.
      if(PreStepResidualKineticEnergy>=fDetectorThreshold && fExpSetup->IsInside(PostStepResidualDirection.Theta(),PostStepResidualDirection.Phi(),PreStepResidualLocation.X(),PreStepResidualLocation.Y(),PreStepResidualLocation.Z())) 
      {
        //The particle identifies a direction within the detector coverage
        //
        //Getting the detector whose direction is identified by the particle
        int TheDetectorIndex = fExpSetup->GetDetectorIndex(PostStepResidualDirection.Theta(),PostStepResidualDirection.Phi(),PreStepResidualLocation.X(),PreStepResidualLocation.Y(),PreStepResidualLocation.Z());
        UNISDetectionUnit * TheDetector = fExpSetup->GetDetector(TheDetectorIndex);
        //Calculation of the distance between the potential hit point and the particle
        TVector3 HitPoint(TheDetector->GetImpactPointLab(PostStepResidualDirection.Theta(),PostStepResidualDirection.Phi(),PreStepResidualLocation.X(),PreStepResidualLocation.Y(),PreStepResidualLocation.Z()));
        TVector3 HitPointRelative = HitPoint-PostStepResidualLocation; //vector identifying the hit point with respect to the post step location
        double HitPointDistance=HitPointRelative.Dot(PostStepResidualDirection); //Projection of the distance vector along the particle direction
        //
        //Evaluation of the distance
        if(fabs(HitPointDistance)<0.1*fCalculationStep) {
          //The particle is at the surface of the detector
          if(fVerbose) {
            printf("*** Particle detected!\n"); 
            printf("**************************************************\n\n");
            getchar();
          }
          fevt->fnumdet[fevt->fmulti]=TheDetectorIndex;
          fevt->fnumpixel[fevt->fmulti]=TheDetector->GetPixel(PostStepResidualDirection.Theta(),PostStepResidualDirection.Phi(),PreStepResidualLocation.X(),PreStepResidualLocation.Y(),PreStepResidualLocation.Z());
          //
          //Check if interstrip
          if(fevt->fnumpixel[fevt->fmulti]>=0) {   
            //Not interstrip event  
            fevt->fmulti_detected++;
            fevt->fIsDetected[fevt->fmulti]=true;
            fevt->fKinEnergy[fevt->fmulti]=PostStepResidualKineticEnergy+ (fDetectorEres>0 ? fRandomGen->Gaus(0,PostStepResidualKineticEnergy*fDetectorEres/2.355) : 0);
            fevt->fXDetHit[fevt->fmulti]=HitPoint.X();
            fevt->fYDetHit[fevt->fmulti]=HitPoint.Y();
            fevt->fZDetHit[fevt->fmulti]=HitPoint.Z();
          }
          //
          break;
        }
        //
        //Check if particle passed the detector without being detected
        if(HitPointDistance<0) {
          //The particle crossed the detector, we undo the last step and perform a step of 1/2 length
          ResidualDetectionStep/=2.;
          //
          //Undo the last step
          continue;
        }
      }
      //
      
      //The particle is not inside or the direction or is inside but the distance is greater than 1/10, I process the next step by using the standard calculation step
      ResidualDetectionStep=fCalculationStep;
      //Finalizing the step -> setting the pre step quantities to the current post step ones
      //WARNING: TEMPORARLY TRACKS ARE NOT DRAWN
      //       if(fGraphics) {
      //         TrackResidual->AddLine(-PreStepResidualLocation.Y(),PreStepResidualLocation.X(),PreStepResidualLocation.Z(),-PostStepResidualLocation.Y(),PostStepResidualLocation.X(),PostStepResidualLocation.Z());
      //       }
      PreStepResidualKineticEnergy=PostStepResidualKineticEnergy;
      PreStepResidualMomentumModule=PostStepResidualMomentumModule;
      PreStepResidualMomentum=PostStepResidualMomentum;
      PreStepResidualDirection=PostStepResidualDirection;
      PreStepResidualLocation=PostStepResidualLocation;
      PreStepResidualMomentumEnergy=PostStepResidualMomentumEnergy;
    } //End of the main iteration for the detection process
    //WARNING: TEMPORARLY TRACKS ARE NOT DRAWN
    //     if(fGraphics) {
    //       gEve->AddElement(TrackResidual);
    //       gEve->Redraw3D();
    //     }
    //    
    
    fevt->fmulti++; //NOTE: in the current version of the program, the multiplicity is always equal to the number of particles detected
    
    //
    //Closing event (writing the event on trees)
    TheTree->Fill();
    //
    
  }
  //End of the Main Iteration
  
  //
  //Running TEveManager standalone
  if(fGraphics) {
    fApp->Run(kTRUE);
    fApp->Terminate(0);
  }
  //
}

//____________________________________________________
void REXppFramework::SteppingActionWindow(TLorentzVector & pre_momentum, TVector3 & pre_location, TLorentzVector & post_momentum, TVector3 & post_location)
{
  //
  //Interaction of the beam with the entrance window
  //
  //Calculation of beam momentum after the interaction with the entrance window
  double PreStepBeamKineticEnergy=pre_momentum.E()-pre_momentum.M();
  TVector3 PreStepBeamDirection(pre_momentum.Vect().Unit());
  
  double AngularStragglingWindow=0.; //NOTE: Here we must add an angular straggling
  //Since we are doing only one step in the entrance window, the lenght of this step will be determined to completely pass the target (including angular straggling)
  double WindowEffectiveThickness=fWindowThickness/cos(AngularStragglingWindow); //Effective distance travelled in the entrance window by the incident beam    
  double ELossEntranceWindow = fSRIMLossModule->GetEnergyLoss(fZbeam,fAbeam,PreStepBeamKineticEnergy,fWindowMaterial.c_str(),WindowEffectiveThickness); //Energy loss of the beam in the entrance window
  double EnergyStragglingWindow=0.; //NOTE: Here we must add an energy straggling
  double PostStepBeamKineticEnergy=PreStepBeamKineticEnergy-ELossEntranceWindow+EnergyStragglingWindow; //calculation of degraded energy
  double PostStepBeamMomentumModule=sqrt(pow(PostStepBeamKineticEnergy+fMassBeam,2)-pow(fMassBeam,2)); //calculation of degraded momentum module
  TVector3 PostStepBeamMomentum=PostStepBeamMomentumModule*PreStepBeamDirection; //calculation of degraded momentum vector
  //
  //Angular straggling of the beam in the entrance window
  PostStepBeamMomentum.Rotate(AngularStragglingWindow,PreStepBeamDirection.Orthogonal()); //angle straggling (rotation around a vector perpendicular to the pre-step vector) 
  PostStepBeamMomentum.Rotate(fRandomGen->Uniform(0,TMath::Pi()),PreStepBeamDirection); //randomization of the straggling direction around the original vector
  TVector3 PostStepBeamDirection(PostStepBeamMomentum.Unit()); //Post-step vector
  //
  //Calculation of beam position after the entrance window, if no straggling occurs -> (0,0,0). In any other case, the beam is located at the exit surface of the foil.
  post_location=pre_location+WindowEffectiveThickness/fcmToum*PostStepBeamDirection;
  //
  //The beam as a quadri-vector after the entrance window
  post_momentum.SetVect(PostStepBeamMomentum);
  post_momentum.SetE(PostStepBeamKineticEnergy+fMassBeam);
  //
  
  if(fVerbose) {
    printf("*** Entrance Window\n");
    printf("material: %s\n", fWindowMaterial.c_str());
    printf("ELoss entrance window: %f MeV\n", ELossEntranceWindow);
    printf("Straggling angle: %f deg\n", AngularStragglingWindow*TMath::DegToRad());
    printf("Momentum pre step: (%f, %f, %f) MeV/c\n", pre_momentum.Vect().X(), pre_momentum.Vect().Y(), pre_momentum.Vect().Z());
    printf("Momentum post step: (%f, %f, %f) MeV/c\n", post_momentum.Vect().X(), post_momentum.Vect().Y(), post_momentum.Vect().Z());
    printf("Post Step location: (%f, %f, %f) cm\n", post_location.X(), post_location.Y(), post_location.Z());
    printf("**************************************************\n\n");
    getchar();
  }
}

//____________________________________________________
void REXppFramework::SteppingActionGas(TLorentzVector & pre_momentum, TVector3 & pre_location, TLorentzVector & post_momentum, TVector3 & post_location, int Z, int A, double step_size)
{
  //
  //Calculation of energy degradation and straggling
  //
  double TheMass = pre_momentum.M(); //Mass of the particle
  double PreStepKineticEnergy=pre_momentum.E()-TheMass; //Kinetic energy before iteration
  TVector3 PreStepDirection(pre_momentum.Vect().Unit()); //Direction of the particle before iteration
  //Energy loss in the current step
  double ELossStep = (Z > 0 ? fSRIMLossModule->GetEnergyLossGas(Z,A,PreStepKineticEnergy,fGasMaterial.c_str(),fGasPressure,fGasTemperature,step_size*fcmToum) : 0); //Energy loss of the beam in the current step of calculation  
  //Energy straggling in the current step
  double EnergyStraggling=0.; //NOTE: Here we must add an energy straggling
  //Angular straggling in the current step
  double AngularStraggling=0.; //NOTE: Here we must add an angular straggling
  //
  //Calculation of the post-step quantities
  double PostStepKineticEnergy=std::fmax(0,PreStepKineticEnergy-ELossStep+EnergyStraggling); //calculation of degraded energy
  double PostStepMomentumModule=sqrt(std::fmax(0,pow(PostStepKineticEnergy+TheMass,2)-pow(TheMass,2))); //calculation of degraded momentum module
  TVector3 PostStepMomentum=PostStepMomentumModule*PreStepDirection; //calculation of degraded momentum vector
  PostStepMomentum.Rotate(AngularStraggling,PreStepDirection.Orthogonal()); //angle straggling (rotation around a vector perpendicular to the pre-step vector) 
  PostStepMomentum.Rotate(fRandomGen->Uniform(0,TMath::Pi()),PreStepDirection); //randomization of the straggling direction around the original vector
  TVector3 PostStepDirection=PostStepMomentum.Unit(); //Post-step vector
  //Calculation of particle position after the calculation step
  post_location=pre_location+step_size*PostStepDirection;
  //Calculation of the energy/momentum
  post_momentum.SetVect(PostStepMomentum);
  post_momentum.SetE(PostStepKineticEnergy+TheMass);
  //
  
  if(fVerbose) {
    printf("*** Calculation Step\n");
    printf("material: %s\n", fGasMaterial.c_str());
    printf("Particle: Z=%d A=%d\n", Z, A);
    printf("ELoss step: %f MeV\n", ELossStep);
    printf("Straggling angle: %f deg\n", AngularStraggling*TMath::RadToDeg());
    printf("Ekin: %f MeV\n", PostStepKineticEnergy);
    printf("Momentum pre step: (%f, %f, %f) MeV/c\n", pre_momentum.Vect().X(), pre_momentum.Vect().Y(), pre_momentum.Vect().Z());
    printf("Momentum post step: (%f, %f, %f) MeV/c\n", post_momentum.Vect().X(), post_momentum.Vect().Y(), post_momentum.Vect().Z());
    printf("Post Step location: (%f, %f, %f) cm\n", post_location.X(), post_location.Y(), post_location.Z());
    printf("**************************************************\n\n");
  }
}

//____________________________________________________
void REXppFramework::PrintPercentage(int jentry, int nentries) const
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
