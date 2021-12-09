#ifndef RELATIVISTICKINEMATICS_H
#define RELATIVISTICKINEMATICS_H

class RelativisticKinematics
{
public:

  RelativisticKinematics();
  ~RelativisticKinematics();
  
  double GetEnergyTwoBodyEjectile(double polar_angle, double E_Projectile, double M_Projectile, double M_Target, double EStar_Projectile=0, double EStar_target=0);
  double GetEnergyTwoBodyRecoil(double polar_angle, double E_Projectile, double M_Projectile, double M_Target, double EStar_Projectile=0, double EStar_target=0);
  double GetEnergyTwoBodyEjectileReaction(double polar_angle, double E_Projectile, double M_Projectile, double M_Target, double M_Projectile_prime, double M_Target_prime, double EStar_Projectile=0, double EStar_target=0);
  double GetEnergyTwoBodyRecoilReaction(double polar_angle, double E_Projectile, double M_Projectile, double M_Target, double M_Projectile_prime, double M_Target_prime, double EStar_Projectile=0, double EStar_target=0);  
  
};

#endif
