#include "PHOTONS++/MEs/Scalar_To_Scalar_Scalar.H"
#include "ATOOLS/Math/Poincare.H"
#include "ATOOLS/Phys/Flavour.H"
#include "ATOOLS/Phys/Particle.H"
#include "METOOLS/Main/Polarization_Tools.H"
#include "METOOLS/Loops/Master_Integrals.H"

#define B_0(A,B,C,M) Master_Bubble(A,B,C,M)

using namespace PHOTONS;
using namespace ATOOLS;
using namespace METOOLS;
using namespace std;

Scalar_To_Scalar_Scalar::Scalar_To_Scalar_Scalar
(const Particle_Vector_Vector& pvv) : PHOTONS_ME_Base(pvv), Dipole_FF(pvv)  {
  m_name = "Scalar_To_Scalar_Scalar";
  m_flavs[0]  = pvv[1][0]->Flav();
  m_masses[0] = pvv[1][0]->FinalMass();
  // switch ordering if necessary
  m_switch = pvv[2][0]->Flav().IsAnti();
  // m_switch == true if first multipole particle is anti
  if (m_switch == false) {
    m_flavs[1] = pvv[2][0]->Flav(); m_masses[1] = pvv[2][0]->FinalMass();
    m_flavs[2] = pvv[2][1]->Flav(); m_masses[2] = pvv[2][1]->FinalMass();
  }
  else {
    m_flavs[2] = pvv[2][0]->Flav(); m_masses[2] = pvv[2][0]->FinalMass();
    m_flavs[1] = pvv[2][1]->Flav(); m_masses[1] = pvv[2][1]->FinalMass();
  }
  for (unsigned int i=3; i<9; i++) {
    m_flavs[i]  = Flavour(kf_photon);
    m_masses[i] = 0.;
  }

  // Hadrons' form factors for basic process
  // set to one, will have to be got from Hadrons
  double F = 1.;
  m_Gamma = m_i*F;
}

Scalar_To_Scalar_Scalar::~Scalar_To_Scalar_Scalar() {
}

void Scalar_To_Scalar_Scalar::BoostOriginalPVVToMultipoleCMS() {
  // m_pvv_one already in multipole CMS
  // m_pvv_zero in arbitrary frame -> boost m_olddipole into its CMS
  // and rotate m_olddipole.at(0) into +z direction
  Vec4D sum(0.,0.,0.,0.);
  for (unsigned int i=0; i<m_olddipole.size(); i++) {
    sum += m_olddipole[i]->Momentum();
  }
  Vec4D p1 = m_olddipole[0]->Momentum();
  p_boost = new Poincare(sum);
  p_boost->Boost(p1);
  p_rot   = new Poincare(p1,Vec4D(0.,0.,0.,1.));
  for (unsigned int i=0; i<m_olddipole.size(); i++) {
    Vec4D vec = m_olddipole[i]->Momentum();
    p_boost->Boost(vec);
    p_rot->Rotate(vec);
    m_olddipole[i]->SetMomentum(vec);
  }
  for (unsigned int i=0; i<m_oldspectator.size(); i++) {
    Vec4D vec = m_oldspectator[i]->Momentum();
    p_boost->Boost(vec);
    p_rot->Rotate(vec);
    m_oldspectator[i]->SetMomentum(vec);
  }
}

void Scalar_To_Scalar_Scalar::FillMomentumArrays
(const Particle_Vector_Vector& pvv_one) {
  // m_moms0 - no photon
  m_moms0[0] = m_pvv_zero[1][0]->Momentum();
  if (m_switch == false) {
    m_moms0[1] = m_pvv_zero[2][0]->Momentum();
    m_moms0[2] = m_pvv_zero[2][1]->Momentum();
  }
  else {
    m_moms0[2] = m_pvv_zero[2][0]->Momentum();
    m_moms0[1] = m_pvv_zero[2][1]->Momentum();
  }
  // m_moms1 - project multiphoton state onto one photon phase space
  // do reconstruction procedure again pretending only one photon was generated

  // not necessary if only one photon
  if (pvv_one[4].size() == 1) {
    m_moms1[0][0] = pvv_one[1][0]->Momentum();
    if (m_switch == false) {
      m_moms1[0][1] = pvv_one[2][0]->Momentum();
      m_moms1[0][2] = pvv_one[2][1]->Momentum();
    }
    else {
      m_moms1[0][2] = pvv_one[2][0]->Momentum();
      m_moms1[0][1] = pvv_one[2][1]->Momentum();
    }
    m_moms1[0][3] = pvv_one[4][0]->Momentum();
  }
  else {
    Dipole_FF::DefineDipole();
    BoostOriginalPVVToMultipoleCMS();
    for (unsigned int i=0; i<pvv_one[4].size(); i++) {
      m_softphotons.push_back(pvv_one[4][i]);
      m_K = CalculateMomentumSum(m_softphotons);
      CorrectMomenta();
      if (m_switch == false) {
        m_moms1[i][1] = m_newdipole[0]->Momentum();
        m_moms1[i][2] = m_newdipole[1]->Momentum();
      }
      else {
        m_moms1[i][2] = m_newdipole[0]->Momentum();
        m_moms1[i][1] = m_newdipole[1]->Momentum();
      }
      m_moms1[i][3] = m_softphotons[0]->Momentum();
      m_moms1[i][0] = m_moms1[i][1]+m_moms1[i][2]+m_moms1[i][3];
      m_softphotons.clear();
    }
  }
}

double Scalar_To_Scalar_Scalar::Smod(unsigned int kk) {
  m_moms = m_moms1[kk];
  double sum = 0;
  Vec4D k   = m_moms[3];
  Vec4D pi  = m_moms[1];
  Vec4D pj  = m_moms[2];
  double Zi = -1.;
  double Zj = +1.;
  int    ti = +1;
  int    tj = +1;
  sum = m_alpha/(4.*M_PI*M_PI)*Zi*Zj*ti*tj*(pi/(pi*k)-pj/(pj*k)).Abs2();
  return sum;
}

Complex Scalar_To_Scalar_Scalar::InfraredSubtractedME_0_0() {
  m_moms = m_moms0;
  return m_Gamma;
}

Complex Scalar_To_Scalar_Scalar::InfraredSubtractedME_0_1() {
  m_moms = m_moms0;
  double mu2 = sqr(m_masses[0]);
  double m2  = sqr(0.5*(m_masses[1]+m_masses[2]));
  return m_alpha/M_PI*m_Gamma*(0.5*B_0(m2,0.,m2,mu2)
                               +0.25*B_0(0.,m2,m2,mu2)).Finite();
}

Complex Scalar_To_Scalar_Scalar::InfraredSubtractedME_0_2() {
  return 0.;
}

Complex Scalar_To_Scalar_Scalar::InfraredSubtractedME_1_05(unsigned int i) {
  m_moms        = m_moms1[i];                // set to set of momenta to be used
  Vec4C epsP    = conj(Polarization_Vector(m_moms[3])[m_spins[3]]);
  Vec4D k1      = m_moms[1];
  Vec4D k2      = m_moms[2];
  Vec4D k       = m_moms[3];
  double m12    = m_masses[1];
  double m22    = m_masses[2];
  Complex rA    = Complex(0.,0.);
  Complex rB    = Complex(0.,0.);
  // radiation off A
  rA = m_e*m_Gamma*((2.*k1+k)*epsP)/((k1+k)*(k1+k)-m12);
  // radiation off B
  rB = m_e*m_Gamma*((2.*k2+k)*epsP)/((k2+k)*(k2+k)-m22);
  return rA+rB;
}

Complex Scalar_To_Scalar_Scalar::InfraredSubtractedME_1_15(unsigned int i) {
  return 0.;
}

Complex Scalar_To_Scalar_Scalar::InfraredSubtractedME_2_1(unsigned int i, unsigned int j) {
  return 0.;
}

double Scalar_To_Scalar_Scalar::GetBeta_0_0() {
  double sum = 0.;
  for (unsigned int i=0; i<=0; i++) {           // spin scalar.Bar
    for (unsigned int j=0; j<=0; j++) {         // spin scalar
      for (unsigned int k=0; k<=0; k++) {       // spin scalar
        m_spins[0] = k;
        m_spins[1] = j;
        m_spins[2] = i;
        Complex M_0_0 = InfraredSubtractedME_0_0();
        sum = sum + (M_0_0*conj(M_0_0)).real();
      }
    }
  }
  // spin avarage over initial state gives factor 1
  return sum;
}

double Scalar_To_Scalar_Scalar::GetBeta_0_1() {
  double sum = 0.;
  for (unsigned int i=0; i<=0; i++) {           // spin scalar.Bar
    for (unsigned int j=0; j<=0; j++) {         // spin scalar
      for (unsigned int k=0; k<=0; k++) {       // spin scalar
        m_spins[0] = k;
        m_spins[1] = j;
        m_spins[2] = i;
        Complex M_0_0 = InfraredSubtractedME_0_0();
        Complex M_0_1 = InfraredSubtractedME_0_1();
        sum = sum + 2.*(M_0_0*conj(M_0_1)).real();
      }
    }
  }
  // spin avarage over initial state gives factor 1
  return sum;
}

double Scalar_To_Scalar_Scalar::GetBeta_0_2() {
  return 0.;
}

double Scalar_To_Scalar_Scalar::GetBeta_1_1(unsigned int a) {
  double sum = 0.;
  for (unsigned int i=0; i<=0; i++) {           // spin scalar.Bar
    for (unsigned int j=0; j<=0; j++) {         // spin scalar
      for (unsigned int k=0; k<=0; k++) {       // spin scalar
        for (unsigned int l=0; l<=1; l++) {     // spin gamma
          m_spins[0] = k;
          m_spins[1] = j;
          m_spins[2] = i;
          m_spins[3] = l;
          Complex M_1_05 = InfraredSubtractedME_1_05(a);
          sum = sum + (M_1_05*conj(M_1_05)).real();
        }
      }
    }
  }
  // spin avarage over initial state gives factor 1
  sum = 1./(16.*M_PI*M_PI*M_PI)*sum - Smod(a)*GetBeta_0_0();
  return sum;
}

double Scalar_To_Scalar_Scalar::GetBeta_1_2(unsigned int i) {
  return 0.;
}

double Scalar_To_Scalar_Scalar::GetBeta_2_2(unsigned int i, unsigned int j) {
  return 0.;
}

// DECLARE_PHOTONS_ME_GETTER(Scalar_To_Scalar_Scalar_Getter,
//                           "Scalar_To_Scalar_Scalar")
// 
// PHOTONS_ME_Base * Scalar_To_Scalar_Scalar_Getter::operator()
// (const Particle_Vector_Vector &pvv) const
// {
//   if ( (pvv.size() == 4) &&
//        (pvv[0].size() == 0) &&
//        (pvv[1].size() == 1) && pvv[1][0]->Flav().IsScalar() &&
//        (pvv[2].size() == 2) && pvv[2][0]->Flav().IsScalar() &&
//                                pvv[2][1]->Flav().IsScalar() &&
//        (pvv[3].size() == 0) )
//     return new Scalar_To_Scalar_Scalar(pvv);
//   return NULL;
// }
