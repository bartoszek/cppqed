// Copyright András Vukics 2006–2020. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt)
#include "Evolution.h"

#include "Qbit.h"

using namespace qbit;
using namespace std;


int main(int argc, char* argv[])
{
  // ****** Parameters of the Problem
  
  ParameterTable p;

  evolution::Pars<> pe(p); // Driver Parameters
  ParsPumpedLossy pplqb(p); 

  // Parameter finalization
  QM_Picture& qmp=updateWithPicture(p,argc,argv);
  
  // ****** ****** ****** ****** ****** ******

  /*
  StateVector psi(state0()()+state1()());
  psi.renorm();
  */
  
  evolve(std::make_shared<StateVector>(init(pplqb)),make(pplqb,qmp),pe);

}

