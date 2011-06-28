// -*- C++ -*-
#ifndef   PROJECTING_MCWF_TRAJECTORY_INCLUDED
#define   PROJECTING_MCWF_TRAJECTORY_INCLUDED

#include "ProjectingMCWF_TrajectoryFwd.h"

#include "MCWF_Trajectory.h"

#include <boost/ptr_container/ptr_vector.hpp>


namespace quantumtrajectory {


template<int RANK>
class ProjectingMCWF_Trajectory : public MCWF_Trajectory<RANK>
{
public:
  typedef MCWF_Trajectory<RANK> Base;

  typedef typename Base::StateVector    StateVector   ;
  typedef typename Base::StateVectorLow StateVectorLow;

  typedef typename Base::QuantumSystem QuantumSystem;

  typedef boost::ptr_vector<StateVector> Basis;

  using Base::getOstream; using Base::toBeAveraged;

  ProjectingMCWF_Trajectory(
			    StateVector& psi,
			    const Basis&,
			    const QuantumSystem& sys,
			    const ParsMCWF_Trajectory&,
			    const StateVectorLow& =StateVectorLow()
			    );

private:
  void   displayEvenMore(int) const;
  size_t displayMoreKey (   ) const;

  const linalg::CMatrix help() const;

  const Basis basis_;
  const linalg::CMatrix metricTensor_uu_;

};


} // quantumtrajectory


#include "impl/ProjectingMCWF_Trajectory.tcc"


#endif // PROJECTING_MCWF_TRAJECTORY_INCLUDED