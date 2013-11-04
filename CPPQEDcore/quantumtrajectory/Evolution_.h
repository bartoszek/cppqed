// -*- C++ -*-
#ifndef QUANTUMTRAJECTORY_EVOLUTION__H_INCLUDED
#define QUANTUMTRAJECTORY_EVOLUTION__H_INCLUDED

#include "Evolution_Fwd.h"

#include "StateVectorFwd.h"
#include "MCWF_TrajectoryFwd.h"
#include "QuantumSystemFwd.h"

#include "SmartPtr.h"
#include "TMP_Tools.h"

#include <iosfwd>



using namespace quantumtrajectory;


std::ostream& operator<<(std::ostream&, EvolutionMode );
std::istream& operator>>(std::istream&, EvolutionMode&);


template<int RANK, typename SYS>
const boost::shared_ptr<MCWF_Trajectory<RANK> > makeMCWF(quantumdata::StateVector<RANK>&, const SYS&, const ParsEvolution&);


template<typename V, int RANK>
void evolve(quantumdata::StateVector<RANK>&, typename structure::QuantumSystem<RANK>::Ptr,
	    const ParsEvolution&);



template<int RANK>
inline
void evolve(quantumdata::StateVector<RANK>& psi,
	    typename structure::QuantumSystem<RANK>::Ptr sys,
	    const ParsEvolution& p)
{
  evolve<tmptools::V_Empty>(psi,sys,p);
}

template<typename V, int RANK>
inline
void evolve(quantumdata::StateVector<RANK>& psi,
	    const structure::QuantumSystem<RANK>& sys,
	    const ParsEvolution& p)
{
  evolve<V>(psi,cpputils::sharedPointerize(sys),p);
}


template<int RANK>
inline
void evolve(quantumdata::StateVector<RANK>& psi,
	    const structure::QuantumSystem<RANK>& sys,
	    const ParsEvolution& p)
{
  evolve<tmptools::V_Empty>(psi,cpputils::sharedPointerize(sys),p);
}


// C++11: use default template argument tmptools::V_Empty to fuse the last two functions

#endif // QUANTUMTRAJECTORY_EVOLUTION__H_INCLUDED