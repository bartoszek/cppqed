// -*- C++ -*-
#ifndef   QUANTUMTRAJECTORY_IMPL_MCWF_TRAJECTORY_TCC_INCLUDED
#define   QUANTUMTRAJECTORY_IMPL_MCWF_TRAJECTORY_TCC_INCLUDED

#include "MCWF_Trajectory.h"

#include "ParsMCWF_Trajectory.h"

#include "StateVector.h"
#include "impl/StochasticTrajectory.tcc"
#include "Structure.h"

#include "impl/FormDouble.tcc"
#include "SmartPtr.h"



namespace quantumtrajectory {

//////////////////
//
// Implementations
//
//////////////////

template<int RANK>
void MCWF_Trajectory<RANK>::derivs(double t, const StateVectorLow& psi, StateVectorLow& dpsidt) const
{
  if (const typename Hamiltonian::Ptr ha=qs_.getHa()) {
    dpsidt=0;

    ha->addContribution(t,psi,dpsidt,tIntPic0_);
    logger_.hamiltonianCalled();
  }
}


template<int RANK> template<typename SYS>
MCWF_Trajectory<RANK>::MCWF_Trajectory(
                                       StateVector& psi,
                                       const SYS& sys,
                                       const ParsMCWF& p,
                                       const StateVectorLow& scaleAbs
                                       )
  : trajectory::Trajectory(p),
    Base(psi(),
         bind(&MCWF_Trajectory::derivs,this,_1,_2,_3),
         trajectory::initialTimeStep(cpputils::sharedPointerize(sys)->highestFrequency()),
         scaleAbs,
         p,
         evolved::MakerGSL<StateVectorLow>(p.sf,p.nextDtTryCorretionFactor),
         randomized::MakerGSL()),
    tIntPic0_(0),
    psi_(psi),
    qs_(cpputils::sharedPointerize(sys),Base::noise()),
    dpLimit_(p.dpLimit), overshootTolerance_(p.overshootTolerance),
    logger_(p.logLevel,qs_.getHa(),getOstream())
{
  if (psi!=*qs_.getQS()) throw DimensionalityMismatchException();
  
  manageTimeStep(qs_.template average<structure::LA_Li>(0.,psi_),getEvolved().get(),false);
  // Initially, dpLimit should not be overshot, either.

}


template<int RANK>
std::ostream& MCWF_Trajectory<RANK>::displayMore() const
{
  using namespace std;

  return qs_.display(getTime(),psi_,getOstream(),getPrecision());

  // if (svdc_ && !(svdCount_%svdc_) && (svdCount_||firstSVDisplay_)) os<<FormDouble(svdPrecision_,0)(psi_()); svdCount_++;
}


template<int RANK>
double MCWF_Trajectory<RANK>::coherentTimeDevelopment(double Dt) const
{
  if (qs_.getHa()) getEvolved()->step(Dt);
  else {
    double stepToDo=qs_.getLi() ? std::min(getDtTry(),Dt) : Dt;
    getEvolved()->update(getTime()+stepToDo,getDtTry());
    logger_.logFailedSteps(getEvolved()->nFailedSteps());
  }

  double t=getTime();

  if (const typename Exact::Ptr ex=qs_.getEx()) {
    ex->actWithU(getDtDid(),psi_());
    tIntPic0_=t;
  }

  logger_.processNorm(psi_.renorm());

  return t;
}


template<int RANK>
const typename MCWF_Trajectory<RANK>::IndexSVL_tuples
MCWF_Trajectory<RANK>::calculateDpOverDtSpecialSet(DpOverDtSet* dpOverDtSet, double t) const
{
  IndexSVL_tuples res;
  for (int i=0; i<dpOverDtSet->size(); i++)
    if ((*dpOverDtSet)(i)<0) {
      StateVector psiTemp(psi_);
      qs_.actWithJ(t,psiTemp(),i);
      res.push_back(IndexSVL_tuple(i,psiTemp()));
      (*dpOverDtSet)(i)=mathutils::sqr(psiTemp.renorm());
    } // psiTemp disappears here, but its storage does not, because the ownership is taken over by the SVL in the tuple
  return res;
}


template<int RANK>
bool MCWF_Trajectory<RANK>::manageTimeStep(const DpOverDtSet& dpOverDtSet, evolved::TimeStepBookkeeper* evolvedCache, bool logControl) const
{
  const double dpOverDt=std::accumulate(dpOverDtSet.begin(),dpOverDtSet.end(),0.);
  const double dtDid=getDtDid(), dtTry=getDtTry();

  // Assumption: overshootTolerance_>=1 (equality is the limiting case of no tolerance)
  if (dpOverDt*dtDid>overshootTolerance_*dpLimit_) {
    evolvedCache->setDtTry(dpLimit_/dpOverDt);
    (*getEvolved())=*evolvedCache;
    logger_.stepBack(dpOverDt*dtDid,dtDid,getDtTry(),getTime(),logControl);
    return true; // Step-back required.
  }
  else if (dpOverDt*dtTry>dpLimit_) {
    // dtTry-adjustment for next step required
    getEvolved()->setDtTry(dpLimit_/dpOverDt);
    logger_.overshot(dpOverDt*dtTry,dtTry,getDtTry(),logControl);
  }
  
  return false; // Step-back not required.
}


template<int RANK>
void MCWF_Trajectory<RANK>::performJump(const DpOverDtSet& dpOverDtSet, const IndexSVL_tuples& dpOverDtSpecialSet, double t) const
{
  double random=(*getRandomized())()/getDtDid();

  int jumpNo=0;
  for (; random>0 && jumpNo!=dpOverDtSet.size(); random-=dpOverDtSet(jumpNo++))
    ;

  if(random<0) { // Jump No. jumpNo-1 occurs
    struct helper
    {
      static bool p(int i, IndexSVL_tuple j) {return i==j.template get<0>();} // NEEDS_WORK how to express this with lambda?
    };

    typename IndexSVL_tuples::const_iterator i(find_if(dpOverDtSpecialSet,bind(&helper::p,--jumpNo,_1))); // See whether it's a special jump
    if (i!=dpOverDtSpecialSet.end())
      // special jump
      psi_()=i->template get<1>(); // RHS already normalized above
    else {
      // normal  jump
      qs_.actWithJ(t,psi_(),jumpNo);
      double normFactor=sqrt(dpOverDtSet(jumpNo));
      if (!boost::math::isfinite(normFactor)) throw structure::InfiniteDetectedException();
      psi_()/=normFactor;
    }

    logger_.jumpOccured(t,jumpNo);
  }
}


template<int RANK>
void MCWF_Trajectory<RANK>::step_v(double Dt) const
{
  const StateVectorLow psiCache(psi_().copy());
  evolved::TimeStepBookkeeper evolvedCache(*getEvolved()); // This cannot be const since dtTry might change.

  double t=coherentTimeDevelopment(Dt);

  if (const typename Liouvillean::Ptr li=qs_.getLi()) {

    DpOverDtSet dpOverDtSet(li->average(t,psi_));
    IndexSVL_tuples dpOverDtSpecialSet=calculateDpOverDtSpecialSet(&dpOverDtSet,t);

    while (manageTimeStep(dpOverDtSet,&evolvedCache)) {
      psi_()=psiCache;
      t=coherentTimeDevelopment(Dt); // the next try
      dpOverDtSet=li->average(t,psi_);
      dpOverDtSpecialSet=calculateDpOverDtSpecialSet(&dpOverDtSet,t);
    }

    // Jump
    performJump(dpOverDtSet,dpOverDtSpecialSet,t);

  }

  logger_.step();

}


template<int RANK>
void MCWF_Trajectory<RANK>::displayParameters_v() const
{
  using namespace std;
  Base::displayParameters_v();

  ostream& os=getOstream();

  os<<"# MCWF Trajectory Parameters: dpLimit="<<dpLimit_<<" (overshoot tolerance factor)="<<overshootTolerance_<<endl<<endl;

  qs_.getQS()->displayParameters(os);

  qs_.displayCharacteristics(os)<<endl;

  if (const typename Liouvillean::Ptr li=qs_.getLi()) {
    os<<"# Decay channels:\n";
    {
      size_t i=0;
      li->displayKey(os,i);
    }
    os<<"# Alternative jumps: ";
    {
      const DpOverDtSet dpOverDtSet(li->average(0,psi_));
      int n=0;
      for (int i=0; i<dpOverDtSet.size(); i++) if (dpOverDtSet(i)<0) {os<<i<<' '; n++;}
      if (!n) os<<"none";
    }
    os<<endl;
  }

}


template<int RANK>
size_t MCWF_Trajectory<RANK>::displayMoreKey() const
{
  size_t i=3;
  qs_.template displayKey<structure::LA_Av>(getOstream(),i);
  return i;
}


} // quantumtrajectory


#endif // QUANTUMTRAJECTORY_IMPL_MCWF_TRAJECTORY_TCC_INCLUDED
