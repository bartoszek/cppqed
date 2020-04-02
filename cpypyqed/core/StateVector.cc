// Copyright Raimar Sandner 2012–2020. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt)
// -*- C++ -*-

#include "Core.h"
#include "blitz2numpy.tcc"

#include "StateVector.tcc"
#include "DensityOperator.tcc"

#include <boost/preprocessor/iteration/local.hpp>

#include <boost/python/import.hpp>

using namespace boost::python;

namespace pythonext{

using quantumdata::StateVector;
using quantumdata::DensityOperator;

template<int RANK>
struct SV_to_python
{
  static PyObject* convert(const StateVector<RANK> &s)
  {
    object qs = import("cpypyqed.tools.quantumstate");
    object StateVector = qs.attr("StateVector");
    return boost::python::incref(StateVector(arrayToNumpy<CArray<RANK>,RANK>(s.getArray())).ptr());
  }
};

template<int RANK>
struct DO_to_python
{
  static PyObject* convert(const DensityOperator<RANK> &d)
  {
    object qs = import("cpypyqed.tools.quantumstate");
    object DensityOperator = qs.attr("DensityOperator");
    return boost::python::incref(DensityOperator(arrayToNumpy<CArray<2*RANK>,2*RANK>(d.getArray())).ptr());
  }
};

void export_22_StateVector()
{

#define BOOST_PP_LOCAL_MACRO(n) \
  to_python_converter<StateVector<n>, SV_to_python<n>>(); \
  to_python_converter<DensityOperator<n>, DO_to_python<n>>(); \
  register_ptr_to_python<boost::shared_ptr<const StateVector<n>>>(); \
  implicitly_convertible<boost::shared_ptr<StateVector<n>>, boost::shared_ptr<const StateVector<n>>>(); \
  register_ptr_to_python<boost::shared_ptr<const DensityOperator<n>>>(); \
  implicitly_convertible<boost::shared_ptr<DensityOperator<n>>, boost::shared_ptr<const DensityOperator<n>>>();
#define BOOST_PP_LOCAL_LIMITS (1, PYTHON_HALF_RANK)
#include BOOST_PP_LOCAL_ITERATE()

}

} // pythonext

