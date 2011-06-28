// -*- C++ -*-
#ifndef _NON_ORTHOGONAL_DENSITY_OPERATOR_H
#define _NON_ORTHOGONAL_DENSITY_OPERATOR_H

#include "NonOrthogonalDensityOperatorFwd.h"

#include "DensityOperator.h"


namespace quantumdata {

template<int RANK>
struct NonOrthogonalDensityOperator/* : DensityOperatorBase<RANK> {

  typedef StateVectorBase<RANK> Base;
  typedef Base::Impl Impl;
  typedef Base::Idx  Idx ;

  NonOrthogonalStateVector(const Impl& array) : Base(array), _arrayPulled(array.shape()), _PulledCompatible(false) {}
  // Not pulled in the first place

  Impl& operator()() {_PulledCompatible=false; return const_cast<Impl&>(static_cast<const Base*>(this)->operator());}

  // matrix-like indexing
  const dcomp matrix(const Idx& i1, const Idx& i2);

  renorm

  CLDO MakeLDO() const; // For this it has to pull

private:
  Impl _arrayPulled;
  bool _PulledCompatible;

  strategyForPulling _p; 

};

			       */ ;

} // quantumdata


#endif // _NON_ORTHOGONAL_DENSITY_OPERATOR_H