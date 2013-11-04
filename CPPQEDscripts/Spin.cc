#include "Evolution.h"
#include "Spin.h"

using namespace std ;


int main(int argc, char* argv[])
{
  // ****** Parameters of the Problem

  ParameterTable p;

  ParsEvolution pe(p); // Driver Parameters

  spin::Pars ps(p);
  
  // Parameter finalization
  update(p,argc,argv,"--");
  
  // ****** ****** ****** ****** ****** ******

  LossySpin spin(ps);
  
  structure::free::StateVector psi(spin.getDimensions());

  psi()(psi().ubound(0))=1;
  
  evolve(psi,spin,pe);

}
