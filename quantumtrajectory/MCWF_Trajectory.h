// -*- C++ -*-
#ifndef QUANTUMTRAJECTORY_MCWF_TRAJECTORY_H_INCLUDED
#define QUANTUMTRAJECTORY_MCWF_TRAJECTORY_H_INCLUDED

#include "MCWF_TrajectoryFwd.h"

#include "StateVectorFwd.h"

#include "MCWF_TrajectoryLogger.h"
#include "Structure.h"

#include "StochasticTrajectory.h"

#include <boost/tuple/tuple.hpp>


namespace quantumtrajectory {


///////////////////////////////////////
//
// Monte Carlo wave-function trajectory
//
///////////////////////////////////////

// NEEDS_WORK factor out template-parameter independent code


#define BASE_class trajectory::Stochastic<typename quantumdata::Types<RANK>::StateVectorLow, const quantumdata::StateVector<RANK>&>

template<int RANK>
class MCWF_Trajectory : public BASE_class
{
public:
  typedef structure::Exact        <RANK> Exact      ;
  typedef structure::Hamiltonian  <RANK> Hamiltonian;
  typedef structure::Liouvillean  <RANK> Liouvillean;
  typedef structure::Averaged     <RANK> Averaged   ;

  typedef quantumdata::StateVector<RANK> StateVector;

  typedef typename StateVector::StateVectorLow StateVectorLow;

  typedef structure::QuantumSystemWrapper<RANK,true> QuantumSystemWrapper;

  typedef BASE_class Base;

#undef  BASE_class

  typedef boost::tuple<int,StateVectorLow> IndexSVL_tuple;

  using Base::getEvolved; using Base::getRandomized; using Base::getDtDid; using Base::getDtTry; using Base::getTime;

  template<typename SYS>
  MCWF_Trajectory(StateVector& psi, const SYS& sys, const ParsMCWF&, const StateVectorLow& =StateVectorLow());

  void derivs(double, const StateVectorLow&, StateVectorLow&) const;

  const StateVector& getPsi() const {return psi_;} 

  const MCWF_Logger& getLogger() const {return logger_;}
  
protected:
  std::ostream&    display_v(std::ostream&, int    ) const;
  std::ostream& displayKey_v(std::ostream&, size_t&) const;
  
  const QuantumSystemWrapper getQS() const {return qs_;}

  cpputils::iarchive&  readState_v(cpputils::iarchive& iar)       {Base:: readState_v(iar) & const_cast<MCWF_Logger &>(logger_); if (qs_.getEx()) tIntPic0_=getTime(); return iar;}
  cpputils::oarchive& writeState_v(cpputils::oarchive& oar) const {return Base::writeState_v(oar) & logger_;}

  std::ostream& logOnEnd_v(std::ostream& os) const {return logger_.onEnd(os);}
  
private:
  typedef std::vector<IndexSVL_tuple> IndexSVL_tuples;
  typedef typename Liouvillean::Probabilities DpOverDtSet;

  void step_v(double); // performs one single adaptive-stepsize MCWF step of specified maximal length

  std::ostream& displayParameters_v(std::ostream&) const;

  const StateVector& toBeAveraged_v() const {return psi_;} 

  double                coherentTimeDevelopment    (                                double Dt);
  const IndexSVL_tuples calculateDpOverDtSpecialSet(      DpOverDtSet* dpOverDtSet, double  t) const;

  bool                  manageTimeStep             (const DpOverDtSet& dpOverDtSet, evolved::TimeStepBookkeeper*, bool logControl=true);

  void                  performJump                (const DpOverDtSet&, const IndexSVL_tuples&, double);
  // helpers to step---we are deliberately avoiding the normal technique of defining such helpers, because in that case the whole MCWF_Trajectory has to be passed

  mutable double tIntPic0_ ; // The time instant of the beginning of the current time step.

  StateVector& psi_;

  const QuantumSystemWrapper qs_;

  const double dpLimit_, overshootTolerance_;

  const MCWF_Logger logger_;

};


} // quantumtrajectory


#endif // QUANTUMTRAJECTORY_MCWF_TRAJECTORY_H_INCLUDED
