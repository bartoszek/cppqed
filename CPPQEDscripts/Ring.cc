#include "EvolutionComposite.h"

#include "ParticleTwoModes.h"


int main(int argc, char* argv[])
{
  ParameterTable p;

  ParsEvolution pe(p);
  particle::Pars pp(p);
  mode::ParsPumpedLossy pmP(p,"P");
  mode::ParsPumpedLossy pmM(p,"M");
  particlecavity::ParsAlong ppcP(p,"P");
  particlecavity::ParsAlong ppcM(p,"M");

  ppcP.modeCav=MFT_PLUS; ppcM.modeCav=MFT_MINUS; 

  update(p,argc,argv,"--");

  pmP.delta-=ppcP.uNot/(isComplex(ppcP.modeCav) ? 1. : 2.);
  pmM.delta-=ppcM.uNot/(isComplex(ppcM.modeCav) ? 1. : 2.);

  QM_Picture qmp=(pe.evol==EM_MASTER || pe.evol==EM_MASTER_FAST) ? QMP_UIP : QMP_IP;
  particle::Ptr part (make(pp ,qmp));
  mode    ::Ptr plus (make(pmP,qmp));
  mode    ::Ptr minus(make(pmM,qmp));

  quantumdata::StateVector<3> psi(init(pp)*
                                  init(pmP)*
                                  init(pmM));

  ParticleAlongCavity pacP(plus ,part,ppcP);
  ParticleAlongCavity pacM(minus,part,ppcM);
  ParticleTwoModes ptm(plus,minus,part,ppcP,ppcM);

  evolve<0>(psi,
            composite::make(
                            Act<1,0>  (pacP),
                            Act<2,0>  (pacM),
                            Act<1,2,0>(ptm)
                            ),
            pe);

}
