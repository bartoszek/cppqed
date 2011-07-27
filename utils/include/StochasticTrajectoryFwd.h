// -*- C++ -*-
#ifndef   _STOCHASTIC_TRAJECTORY_FWD_H
#define   _STOCHASTIC_TRAJECTORY_FWD_H


namespace trajectory {

struct ParsStochasticTrajectory;

template<typename T> 
class StochasticTrajectoryBase;

template<typename T, typename T_ELEM=T>
class EnsembleTrajectories;

template<typename T, typename T_ELEM>
class EnsembleTrajectoriesTraits;


template<typename A, typename T> 
class StochasticTrajectory;


}

#endif // _STOCHASTIC_TRAJECTORY_FWD_H