{
//    TStripDetector pollame(0,10,10,-TMath::DegToRad()*90,16,0.2,0.01,0.2,0.2);
//   TStripDetector pollame(15,0.*TMath::DegToRad(),0.,16,0.2,0.01,0.2,0.2);
  
  TStripCluster pollame;
  pollame.AddDetector(0,10,10,-TMath::DegToRad()*90,16,0.2,0.01,0.2,0.2);
  pollame.AddDetector(15,20.*TMath::DegToRad(),0.,16,0.2,0.01,0.2,0.2);
  
  pollame.Draw3D();  
}
