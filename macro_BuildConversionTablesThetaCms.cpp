//#define GRAPHICAL_MODE

/* macro_BuildConversionTablesThetaCms
 * This macro creates the Edet -> ThetaCms conversion tables.
 * Interpolations ThetaCms vs Edet distributions are done. The output is written on file.
 * 
 * NOTE: In simulated data detectors start always from 0. First front strip is at the left, first back strip is at the top.
 * 
 * 29/03/2020
 * v 1.1 
 *  - Inserted smart detector definition
 *  - Smart tree reading  
 * 
 */

#include "include/REXppRootEvent.h"

//
const int NUM_STRIPS_FRONT=16;
const int NUM_STRIPS_BACK=16;
//

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

int RetrieveFromFile(const char * file_name, std::vector<TGraph ***> TheTables) 
{
  TFile * FileInHisto = new TFile(file_name);
  if(FileInHisto->IsZombie()) return 0;
  
  TIter next(FileInHisto->GetListOfKeys());
  TKey *key;
  
  while ((key = (TKey*)next()))
  {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TGraph")) continue;
    TGraph *graph = (TGraph*)key->ReadObj();
   
    std::string GraphName(graph->GetName());
            
    std::string DSSSD_num(GraphName.substr(GraphName.find("DSSSD")+5,GraphName.find("_FRONT")-(GraphName.find("DSSSD")+5)));
    std::string FRONT_num(GraphName.substr(GraphName.find("FRONT")+5,GraphName.find("_BACK")-(GraphName.find("FRONT")+5)));
    std::string BACK_num(GraphName.substr(GraphName.find("BACK")+4));
    
    int TheDSSSD = std::stoi(DSSSD_num);
    int TheFRONT = std::stoi(FRONT_num);
    int TheBACK = std::stoi(BACK_num);
                
    TheTables[TheDSSSD][TheFRONT][TheBACK] = (TGraph*) FileInHisto->Get(Form("DSSSD%02d_FRONT%02d_BACK%02d",TheDSSSD,TheFRONT,TheBACK));
  }
  
  return 1;
}

void WriteOnFile(const char * file_name, std::vector<TGraph ***> TheTables)
{
  TFile * FileOutGraph = new TFile(file_name,"RECREATE");
  
  // Allocation of graphs in case this is the first occurrence of this Ex
  const int NUM_DET = TheTables.size();
  for(int i=0; i<NUM_DET; i++) {
    for(int j=0; j<NUM_STRIPS_FRONT; j++) {
      for(int k=0; k<NUM_STRIPS_BACK; k++) {
        if(TheTables[i][j][k]==0) continue;
        FileOutGraph->WriteTObject(TheTables[i][j][k],TheTables[i][j][k]->GetName());
      }
    }
  }
  // End of allocation loop
  
  FileOutGraph->Close();
  
  return;
}

void macro_BuildConversionTablesThetaCms()
{
  const double GasPressure=689; //mbar //In the LNL experiment: 873 mbar, 869 mbar, 579 mbar, 253 mbar, 243 mbar, 355 mbar, 690 mbar
  const double GasTemperature=298.63; //K
  const std::string GasMaterial("He"); // gas material
  const std::string WindowMaterial("Havar"); //material of the entrance gas window
  const double WindowThickness=2.25; //Thickness of the solid window in um
  const double BeamEnergy=58; //MeV //In the LNL experiment: 69 MeV, 59 MeV, 48 MeV
  const int Zbeam=10;
  const int Abeam=20;
  const int Ztarget=2;
  const int Atarget=4;
  const int Zparticle=2;
  const int Aparticle=4;
  const std::string EntranceChannel("Ne20_He4");
  const std::string ExitChannel("Ne20_He4");
  const std::string KinematicSolution(""); //In the case of double kinematic solution, use "Hi" or "Low" respectively for the high or low energy solution
  const double Exresidual=0.; // Excitation energy populated in the heavy residual //In the LNL experiment is 20Ne
  //
  //Telescopes configuration
  std::map <int, int> TELESCOPE_NUMBER; //Association simulation -> experiment telescope numbers
  std::map <int, std::string> FRONT_STRIP_ORDERING; //Ordering of front strips "leftright" or "rightleft"
  std::map <int, std::string> BACK_STRIP_ORDERING; //Ordering of back strips "topbottom" or "bottomtop"
  TELESCOPE_NUMBER[0]=5;
  FRONT_STRIP_ORDERING[0]=std::string("rightleft");
  BACK_STRIP_ORDERING[0]=std::string("topbottom");
  const int NUM_TELESCOPES=TELESCOPE_NUMBER.size();
  //
  //
  
  //
  //Output on a text file, opening file
  std::string FileName;
  std::string FileBufferName;
  if(KinematicSolution.compare("Hi")==0 || KinematicSolution.compare("Low")==0) {
    FileName.assign(Form("output/REXpp_ConversionParametersThetaCms_%s_%s_Z%02d_A%02d_%s%.2fum_%.0fmbar%.2fK_%.0fMeV_%s.dat",EntranceChannel.c_str(),ExitChannel.c_str(),Zparticle, Aparticle, WindowMaterial.c_str(),WindowThickness,GasPressure,GasTemperature,BeamEnergy,KinematicSolution.c_str()));
    FileBufferName.assign(Form("/mnt/nas0/dellaquila/201904_Mg24/output/REX++/REXpp_ConversionBufferThetaCms_%s_%s_Z%02d_A%02d_%s%.2fum_%.0fmbar%.2fK_%.0fMeV_%s.root",EntranceChannel.c_str(),ExitChannel.c_str(),Zparticle, Aparticle, WindowMaterial.c_str(),WindowThickness,GasPressure,GasTemperature,BeamEnergy,KinematicSolution.c_str()));
  } else {
    FileName.assign(Form("output/REXpp_ConversionParametersThetaCms_%s_%s_Z%02d_A%02d_%s%.2fum_%.0fmbar%.2fK_%.0fMeV.dat",EntranceChannel.c_str(),ExitChannel.c_str(),Zparticle, Aparticle, WindowMaterial.c_str(),WindowThickness,GasPressure,GasTemperature,BeamEnergy));
    FileBufferName.assign(Form("/mnt/nas0/dellaquila/201904_Mg24/output/REX++/REXpp_ConversionBufferThetaCms_%s_%s_Z%02d_A%02d_%s%.2fum_%.0fmbar%.2fK_%.0fMeV.root",EntranceChannel.c_str(),ExitChannel.c_str(),Zparticle, Aparticle, WindowMaterial.c_str(),WindowThickness,GasPressure,GasTemperature,BeamEnergy));
  }
  std::ofstream FileOut (FileName.c_str());
  FileOut << "*" << std::setw(20) << "detector" << std::setw(20) << "strip_front" << std::setw(20) << "strip_back" << std::setw(20) << "minimu_Edet" << std::setw(20) << "maximum_Edet" << std::setw(20) << "num_points" << std::setw(20) << "(Edet, ThetaCms) ..." << std::endl; 
  //
  
  //
  //Opening data
  const std::string DataFolder (Form("/mnt/nas0/dellaquila/201904_Mg24/output/REX++/REXpp_%s_%s_%s%.2fum_%.0fmbar%.2fK_%.0fMeV/",EntranceChannel.c_str(),ExitChannel.c_str(),WindowMaterial.c_str(),WindowThickness,GasPressure,GasTemperature,BeamEnergy));
  const std::string FileInName(Form("%sREXpp_%s_%s_%s%.2fum_%.0fmbar%.2fK_%.0fMeV_Ex*.root",DataFolder.c_str(),EntranceChannel.c_str(),ExitChannel.c_str(),WindowMaterial.c_str(),WindowThickness,GasPressure,GasTemperature,BeamEnergy));
  //
  //Initializing reader
  TChain * TreeDetected = new TChain("t");
  TreeDetected->Add(FileInName.c_str());
  TTreeReader TheReader(TreeDetected);
  TTreeReaderValue <REXppRootEvent> TheEvent(TheReader,"cluster.");
  //
  
  //
  //Data structures for histograms
  //For each excitation energy one has one histogram for each front-back crossing of each telescope
  //
  std::map<double,TH1D ****> HistoKinEnergy; 
  std::map<double,TH1D ****> HistoThetaOriginCms;
  //
  
  //
  //See if the plots where previously buffered to a file
  std::vector<TGraph ***> ConversionTable;
  for(int i=0; i<NUM_TELESCOPES; i++) {
    TGraph *** NewDetector = new TGraph**[NUM_STRIPS_FRONT];
    for(int j=0; j<NUM_STRIPS_FRONT; j++) {
      NewDetector[j]=new TGraph*[NUM_STRIPS_BACK];
      for(int k=0; k<NUM_STRIPS_BACK; k++) {
        NewDetector[j][k]=0;
      }
    }
    ConversionTable.push_back(NewDetector);
  }
  const bool IsBuffered = RetrieveFromFile(FileBufferName.c_str(),ConversionTable);
  //
  
  //
  if(!IsBuffered) { //If we didn't write down a buffer of histograms before let's loop on data
    //
    //Loop on data
    Long64_t nentries = TreeDetected->GetEntries();
    for(Long64_t ientry=0; TheReader.Next(); ientry++) {
      
      if(ientry%100==0) {
        PrintPercentage(ientry, nentries);
      }    
      
      REXppRootEvent * TheDetectors = TheEvent.Get();
      
      // Allocation of histograms in case this is the first occurrence of this Ex
      if(HistoKinEnergy.find(TheDetectors->fEx)==HistoKinEnergy.end()) {
        (HistoKinEnergy[TheDetectors->fEx])= new TH1D ***[NUM_TELESCOPES];
        (HistoThetaOriginCms[TheDetectors->fEx])= new TH1D ***[NUM_TELESCOPES];
        for(int i=0; i<NUM_TELESCOPES; i++) {
          (HistoKinEnergy[TheDetectors->fEx])[i]= new TH1D **[NUM_STRIPS_FRONT];
          (HistoThetaOriginCms[TheDetectors->fEx])[i]= new TH1D **[NUM_STRIPS_FRONT];
          for(int j=0; j<NUM_STRIPS_FRONT; j++) {
            (HistoKinEnergy[TheDetectors->fEx])[i][j]= new TH1D *[NUM_STRIPS_BACK];
            (HistoThetaOriginCms[TheDetectors->fEx])[i][j]= new TH1D *[NUM_STRIPS_BACK];
            for(int k=0; k<NUM_STRIPS_BACK; k++) {
              (HistoKinEnergy[TheDetectors->fEx])[i][j][k]= new TH1D (Form("KinEnergy_Ex%.1f_DSSSD%02d_FRONT%02d_BACK%02d",TheDetectors->fEx,i,j,k),Form("KinEnergy_Ex%.1f_DSSSD%02d_FRONT%02d_BACK%02d",TheDetectors->fEx,i,j,k),100,0.,70);
              (HistoThetaOriginCms[TheDetectors->fEx])[i][j][k]= new TH1D (Form("ThetaOriginCms_Ex%.1f_DSSSD%02d_FRONT%02d_BACK%02d",TheDetectors->fEx,i,j,k),Form("ThetaOriginCms_Ex%.1f_DSSSD%02d_FRONT%02d_BACK%02d",TheDetectors->fEx,i,j,k),100,0.,70);
            }
          }
        }
      }
      // End of allocation loop (only at the first occurrence of Ex)
      
      //
      for(int i=0; i<TheDetectors->fmulti; i++) {
        if(TheDetectors->fZ[i]==Zparticle && TheDetectors->fA[i]==Aparticle) {
          if(TheDetectors->fnumdet[i]>=0 && TheDetectors->fnumstripf[i]>=0 && TheDetectors->fnumstripb[i]>=0) {
            if(KinematicSolution.compare("Hi")==0) {
              if(TheDetectors->fThetaOriginCms[i]>TMath::Pi()/2) continue;
            } else if(KinematicSolution.compare("Low")==0) {
              if(TheDetectors->fThetaOriginCms[i]<=TMath::Pi()/2) continue;
            }
            (HistoKinEnergy[TheDetectors->fEx])[TheDetectors->fnumdet[i]][TheDetectors->fnumstripf[i]][TheDetectors->fnumstripb[i]]->Fill(TheDetectors->fKinEnergy[i]);
            (HistoThetaOriginCms[TheDetectors->fEx])[TheDetectors->fnumdet[i]][TheDetectors->fnumstripf[i]][TheDetectors->fnumstripb[i]]->Fill(TheDetectors->fThetaOriginCms[i]);
          }
        }
      }
      //
    }
    
    //
    // Calculation of Ex vs Edet for each pixel
    //
    std::vector<double> ThetaOriginCmsValues[NUM_TELESCOPES][NUM_STRIPS_FRONT][NUM_STRIPS_BACK];
    std::vector<double> EdetValues[NUM_TELESCOPES][NUM_STRIPS_FRONT][NUM_STRIPS_BACK];
    for(std::map<double,TH1D ****>::iterator it = HistoKinEnergy.begin(); it != HistoKinEnergy.end(); it++)
    {
      for(int i=0; i<NUM_TELESCOPES; i++) {
        for(int j=0; j<NUM_STRIPS_FRONT; j++) {
          for(int k=0; k<NUM_STRIPS_BACK; k++) {
            if((it->second)[i][j][k]->Integral()==0) continue;
            #ifdef GRAPHICAL_MODE
//             (it->second)[i][j][k]->Draw();
//             gPad->Modified();
//             gPad->Update();
            //           getchar();
            #endif
            EdetValues[i][j][k].push_back((it->second)[i][j][k]->GetMean());
            ThetaOriginCmsValues[i][j][k].push_back(HistoThetaOriginCms[it->first][i][j][k]->GetMean());
          }
        }
      }
    }
    //
    
    //
    //Construction of graphs
    for(int i=0; i<NUM_TELESCOPES; i++) {
      for(int j=0; j<NUM_STRIPS_FRONT; j++) {
        for(int k=0; k<NUM_STRIPS_BACK; k++) {
          if(EdetValues[i][j][k].size()==0) {
            continue;
          }
          ConversionTable[i][j][k]=new TGraph(EdetValues[i][j][k].size(),EdetValues[i][j][k].data(),ThetaOriginCmsValues[i][j][k].data());
          ConversionTable[i][j][k]->SetTitle(Form("DSSSD%02d_FRONT%02d_BACK%02d;E_{det} (MeV);#theta_{cm} (rad)",i,j,k));
          ConversionTable[i][j][k]->SetName(Form("DSSSD%02d_FRONT%02d_BACK%02d",i,j,k));
        }
      }
    }
    //
    
    //
    //Writing histograms to buffer file
    WriteOnFile(FileBufferName.c_str(),ConversionTable);
  }
  //
  
  //
  // Saving points to text file
  //
  for(int i=0; i<NUM_TELESCOPES; i++) {
    for(int j=0; j<NUM_STRIPS_FRONT; j++) {
      for(int k=0; k<NUM_STRIPS_BACK; k++) {
        if(ConversionTable[i][j][k]==0) continue;
        ConversionTable[i][j][k]->SetLineColor(kRed);
        double minimum_constraint = *std::min_element(ConversionTable[i][j][k]->GetX(), ConversionTable[i][j][k]->GetX()+ConversionTable[i][j][k]->GetN());
        double maximum_constraint = *std::max_element(ConversionTable[i][j][k]->GetX(), ConversionTable[i][j][k]->GetX()+ConversionTable[i][j][k]->GetN());
        FileOut << std::setw(21) << TELESCOPE_NUMBER[i] << std::setw(20) << (FRONT_STRIP_ORDERING[i].compare("rightleft")==0 ? NUM_STRIPS_FRONT-1-j : j) << std::setw(20) << (BACK_STRIP_ORDERING[i].compare("bottomtop")==0 ? NUM_STRIPS_BACK-1-k : k) << std::setw(20) << minimum_constraint << std::setw(20) << maximum_constraint << std::setw(20) << ConversionTable[i][j][k]->GetN();
        double * Edet_coordinate = ConversionTable[i][j][k]->GetX();
        double * ThetaOriginCms_coordinate = ConversionTable[i][j][k]->GetY();
        for(int n_point=0; n_point<ConversionTable[i][j][k]->GetN(); n_point++) {
          FileOut << std::setw(20) << Edet_coordinate[n_point] << std::setw(20) << ThetaOriginCms_coordinate[n_point];
        }
        FileOut << std::endl;        
#ifdef GRAPHICAL_MODE
        ConversionTable[i][j][k]->SetMarkerStyle(20);
        ConversionTable[i][j][k]->Draw("APL");
        ConversionTable[i][j][k]->GetXaxis()->SetLimits(0,30);
        ConversionTable[i][j][k]->GetYaxis()->SetRangeUser(10,20);
        gPad->Modified();
        gPad->Update();
//         getchar();
        
#endif
      }
    }
  }
  //
  
  //
  //Closing output file
  FileOut.close();
  //
  
  return;  
}
