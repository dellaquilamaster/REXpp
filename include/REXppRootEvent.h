#ifndef REXPPROOTEVENT
#define REXPPROOTEVENT

#include <TROOT.h>

class REXppRootEvent
{
public :
  REXppRootEvent(); //!
  ~REXppRootEvent(); //!
  
  Int_t fmulti;
  Int_t fmulti_detected;
  Double_t fEx; 
  Double_t fExEffective; 
  Bool_t * fIsDetected; //[fmulti]
  Bool_t * fIsStopped; //[fmulti]
  Int_t * fnumdet; //[fmulti]
  Int_t * fnumpixel; //[fmulti]
  Double_t * fKinEnergy; //[fmulti]
  Double_t * fXDetHit; //[fmulti]
  Double_t * fYDetHit; //[fmulti]
  Double_t * fZDetHit; //[fmulti]
  Double_t * fKinEnergyOrigin; //[fmulti]
  Double_t * fThetaOrigin; //[fmulti]
  Double_t * fPhiOrigin; //[fmulti]
  Double_t * fKinEnergyOriginCms; //[fmulti]
  Double_t * fThetaOriginCms; //[fmulti]
  Int_t * fZ; //[fmulti]
  Int_t * fA; //[fmulti]

  ClassDef(REXppRootEvent,2);
};

#endif
