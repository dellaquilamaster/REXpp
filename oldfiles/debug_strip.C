{
  TStripCluster DetectorCluster;
  DetectorCluster.AddDetector(0.,0.,15.,0.,16,0.2,0.01,0.2,0.2); //DSSSD 0 at (0,0,15), 0 deg tilt angle, 16 strips, 0.2 strip width, 0.01 inter-strip width, 0.2 frame width, 0.2 dead layer

  TH2D * pallettoni = new TH2D("pallettoni","",300,-2,2,300,-2,2);
  for (int i=0; i<1000000; i++) {
    double theta=acos(gRandom->Uniform(0,1));
    double phi=gRandom->Uniform(0,2*TMath::Pi());
    if(DetectorCluster.IsInside(theta,phi,0,0,14)) {
      TStripDetector * TheDetector = DetectorCluster.GetDetector(theta,phi,0,0,14);
      
      int nfront = TheDetector->GetStripFront(theta,phi,0,0,14);
      int nback = TheDetector->GetStripBack(theta,phi,0,0,14);
      
      if (nfront<0 || nback<0) continue;
      
      if(nfront!=8 || nback!=9) continue;
      
      TVector3 impatto=TheDetector->GetImpactPointLab(theta,phi,0,0,14);
      pallettoni->Fill(impatto.Y(),impatto.X());
    }
  }
  
  pallettoni->Draw("col");
  
}
