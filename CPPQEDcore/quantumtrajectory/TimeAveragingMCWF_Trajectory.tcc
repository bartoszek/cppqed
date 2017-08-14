// Copyright András Vukics 2006–2017. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt)
#ifndef   CPPQEDCORE_QUANTUMTRAJECTORY_TIMEAVERAGINGMCWF_TRAJECTORY_TCC_INCLUDED
#define   CPPQEDCORE_QUANTUMTRAJECTORY_TIMEAVERAGINGMCWF_TRAJECTORY_TCC_INCLUDED

#include "TimeAveragingMCWF_Trajectory.h"

#include "MCWF_Trajectory.tcc"


template<int RANK>
std::ostream& quantumtrajectory::TimeAveragingMCWF_Trajectory<RANK>::display_v(std::ostream& os, int precision) const
{
  if (av_) {
    Averages averagesNow(av_->average(this->getTime(),this->getPsi()));
    if (this->getTime()>relaxationTime_) {
      averages_ = ( averages_*sum_ + averagesNow )/(sum_+1) ;
      ++sum_;
    }
    av_->process(averagesNow);
    av_->display(averagesNow,os,precision);
  }
  return os;
}


template<int RANK>
std::ostream& quantumtrajectory::TimeAveragingMCWF_Trajectory<RANK>::logMoreOnEnd_v(std::ostream& os) const
{
  if (av_) {
    Averages endValues(averages_.copy());
    // process would “corrupt” the averages_, which would be incorrectly saved afterwards by writeState_v, therefore we process a copy instead
    av_->process(endValues);
    av_->display(endValues,os<<"Time averages after relaxation time "<<relaxationTime_<<std::endl,FormDouble::overallPrecision)<<std::endl;
  }
  return Base::logMoreOnEnd_v(os);
}


#endif // CPPQEDCORE_QUANTUMTRAJECTORY_TIMEAVERAGINGMCWF_TRAJECTORY_TCC_INCLUDED
