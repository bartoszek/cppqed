// Copyright András Vukics 2006–2017. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt)
#include "TimeIndependentMatrixHamiltonian.h"


class MatrixNotSquareException : public cpputils::Exception {};


TimeIndependentMatrixHamiltonian::TimeIndependentMatrixHamiltonian(const CMatrix& hamiltonianOverI)
  : structure::Free(hamiltonianOverI.extent(0),RF{"Largest frequency",max(abs(hamiltonianOverI)),1.}),
    hamiltonianOverI_(hamiltonianOverI.copy())
{
  if (hamiltonianOverI_.extent(0)!=hamiltonianOverI_.extent(1)) throw MatrixNotSquareException();
  getParsStream()<<"# Time-independent matrix Hamiltonian"<<std::endl;
}


void TimeIndependentMatrixHamiltonian::addContribution_v(structure::NoTime, const StateVectorLow& psi, StateVectorLow& dpsidt) const
{
  linalg::apply(psi,dpsidt,hamiltonianOverI_);
}
