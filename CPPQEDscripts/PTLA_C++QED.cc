// Copyright András Vukics 2006–2017. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt)
#include "Evolution.h"

#include "PumpedTwoLevelAtom.h"
#include "Qbit.h"

using namespace std;
using namespace qbit;


int main(int argc, char* argv[])
{
  // ****** Parameters of the Problem

  ParameterTable p;

  evolution::Pars pe(p); // Driver Parameters
  ParsPumpedLossyPhaseNoise pp2la(p);
  
  auto& fullImpl=p.addTitle("Script specific").add("fullImpl","Implement with Qbit (instead of PumpedTwoLevelAtomSch)",false);

  // Parameter finalization
  QM_Picture& qmp=updateWithPicture(p,argc,argv);
  
  // ****** ****** ****** ****** ****** ******

  structure::Free::Ptr atom(fullImpl ? boost::static_pointer_cast<const structure::Free>(make(pp2la,qmp)) : 
                                       boost::static_pointer_cast<const structure::Free>(boost::make_shared<const PumpedTwoLevelAtomSch>(pp2la)));
  
  StateVector psi(init(pp2la));

  evolve(psi,atom,pe);




}

