#include <REXppRootEvent.h>

//____________________________________________________
REXppRootEvent::REXppRootEvent() :
fmulti(0),
fIsDetected(new Bool_t[10]),
fIsStopped(new Bool_t[10]),
fnumdet(new Int_t[10]),
fnumpixel(new Int_t[10]),
fKinEnergy(new Double_t[10]),
fXDetHit(new Double_t[10]),
fYDetHit(new Double_t[10]),
fZDetHit(new Double_t[10]),
fKinEnergyOrigin(new Double_t[10]),
fThetaOrigin(new Double_t[10]),
fPhiOrigin(new Double_t[10]),
fKinEnergyOriginCms(new Double_t[10]),
fThetaOriginCms(new Double_t[10]),
fZ(new Int_t[10]),
fA(new Int_t[10])
{}

//____________________________________________________
REXppRootEvent::~REXppRootEvent()
{
  delete [] fIsDetected;
  delete [] fIsStopped;
  delete [] fnumdet;
  delete [] fnumpixel;
  delete [] fKinEnergy;
  delete [] fXDetHit;
  delete [] fYDetHit;
  delete [] fZDetHit;
  delete [] fKinEnergyOrigin;
  delete [] fThetaOrigin;
  delete [] fPhiOrigin;
  delete [] fKinEnergyOriginCms;
  delete [] fThetaOriginCms;
  delete [] fZ;
  delete [] fA;
}
