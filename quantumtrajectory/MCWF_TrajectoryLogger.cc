#include "MCWF_TrajectoryLogger.h"

#include <iostream>
#include <cmath>


using namespace std;


quantumtrajectory::MCWF_Logger::MCWF_Logger(unsigned logLevel, bool isHamiltonian, std::ostream& os)
  : logLevel_(logLevel), isHamiltonian_(isHamiltonian), os_(os), nSteps_(), nOvershot_(), nToleranceOvershot_(), nFailedSteps_(), nHamiltonianCalls_(), dpMaxOvershoot_(), dpToleranceMaxOvershoot_(), normMaxDeviation_(), traj_()
{}


quantumtrajectory::MCWF_Logger::~MCWF_Logger()
{
  if (logLevel_) {
    os_
      <<"\n# Total number of steps: "<<nSteps_<<endl
      <<"\n# dpLimit overshot: "<<nOvershot_<<" times, maximal overshoot: "<<dpMaxOvershoot_
      <<"\n# dpTolerance overshot: "<<nToleranceOvershot_<<" times, maximal overshoot: "<<dpToleranceMaxOvershoot_<<endl
      <<"\n# Maximal deviation of norm from 1: "<<normMaxDeviation_<<endl;
    if (isHamiltonian_)
      os_
	<<"\n# Failed ODE steps: "<<nFailedSteps_
	<<"\n# Number of Hamiltonian calls: "<<nHamiltonianCalls_<<endl;
    os_
      <<"\n# MCWF Trajectory:\n";
      
    for (MCWF_Trajectory::const_iterator i=traj_.begin(); i!=traj_.end(); ++i) os_<<"# "<<i->first<<"\t"<<i->second<<std::endl;
      
    // NEEDS_WORK the lambda expression of old doesn’t work with c++0x
    // boost::for_each(traj_,os_<<constant("# ")<<bind(&pair<double,size_t>::first,_1)<<constant("\t")<<bind(&pair<double,size_t>::second,_1)<<constant("\n"));
  }
}


void quantumtrajectory::MCWF_Logger::step() const {++nSteps_;}


void quantumtrajectory::MCWF_Logger::stepBack(double dp, double dtDid, double newDtTry, double t, bool logControl) const
{
  ++nToleranceOvershot_;
  if (logControl) dpToleranceMaxOvershoot_=max(dpToleranceMaxOvershoot_,dp);
  if (logLevel_>2)
    os_<<"# dpTolerance overshot: "<<dp<<" stepping back to "<<t<<" timestep decreased: "<<dtDid<<" => "<<newDtTry<<endl;
}


void quantumtrajectory::MCWF_Logger::overshot(double dp, double oldDtTry, double newDtTry, bool logControl) const
{
  ++nOvershot_;
  if (logControl) dpMaxOvershoot_=max(dpMaxOvershoot_,dp);
  if (logLevel_>2)
    os_<<"# dpLimit overshot: "<<dp<<" timestep decreased: "<<oldDtTry<<" => "<<newDtTry<<endl;
}


void quantumtrajectory::MCWF_Logger::processNorm(double norm) const
{
  normMaxDeviation_=max(normMaxDeviation_,fabs(1-norm));
  // NEEDS_WORK this should be somehow weighed by the timestep
}


void quantumtrajectory::MCWF_Logger::jumpOccured(double t, size_t jumpNo) const
{
  traj_.push_back(make_pair(t,jumpNo));
  if (logLevel_>1)
    os_<<"# Jump No. "<<jumpNo<<" at time "<<t<<endl;
}


void quantumtrajectory::MCWF_Logger::logFailedSteps(size_t n) const
{
  nFailedSteps_+=n;
  if (logLevel_>3)
    os_<<"# Number of failed steps in this timestep: "<<n<<endl;
}


void quantumtrajectory::MCWF_Logger::hamiltonianCalled() const
{
  nHamiltonianCalls_++;
}
