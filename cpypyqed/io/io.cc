// Copyright Raimar Sandner 2012–2020. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt)
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <iostream>
#include "Evolved.h"
#include "PythonExtension.h"

#include "Types.h"

#include "BlitzArrayTraits.h"
#include "BlitzTiny.h"
#include "Trajectory.tcc"

#include "blitz2numpy.tcc"
#include "utils.h"

#if PYTHON_MAX_RANK > BLITZ_ARRAY_LARGEST_RANK
#define BLITZ_ARRAY_LARGEST_RANK PYTHON_MAX_RANK
#endif

#include <algorithm>
#include <blitz/tinyvec2.h>
#include <boost/lexical_cast.hpp>
#include <boost/preprocessor/iteration/local.hpp>
#include <boost/python/exception_translator.hpp>
#include <numpy/ndarrayobject.h>
#include <fstream>
#include <string>

using namespace trajectory;
using namespace boost::python;

namespace pythonext {

namespace {

template<typename A, int RANK>
object doRead(std::istream *ifs)
{
  list states;
  list times;
  list result;
  ExtTiny<RANK> dims;
  dims=0;
  A a(dims);
  AdaptiveIO<A> traj(evolved::makeIO(a));
  while ( (ifs->peek(), !ifs->eof()) ) {
    trajectory::readViaSStream(traj,ifs);
    states.append(arrayToNumpy<A,RANK>(a));
    times.append(traj.getTime());
  }
  return make_tuple(states,times);
}

template<typename A, typename dtype, int RANK>
void doWrite(std::ostream *ofs, const numeric::array &a, double time)
{
  A  blitz_a = numpyToArray<dtype,RANK>(a);
  AdaptiveIO<A> traj(evolved::makeIO(blitz_a, time));
  trajectory::writeViaSStream(traj,ofs);
}

void throw_rank(int r)
{
  using std::string;
  using boost::lexical_cast;
  if (r>PYTHON_MAX_RANK){
    PyErr_SetString(PyExc_NotImplementedError,
                    (lexical_cast<string>(r)+string(">PYTHON_MAX_RANK=")+lexical_cast<string>(PYTHON_MAX_RANK)).c_str());
    throw_error_already_set();
  }
}

void throw_type(std::string typeID)
{
  if(typeID!="CArray" && typeID!="DArray") {
    PyErr_SetString(PyExc_NotImplementedError, (typeID+": import of this data type not implemented.").c_str());
    throw_error_already_set();
  }
}

}

object read(str filename) 
{
  std::string f = extract<std::string>(filename);

  trajectory::SerializationMetadata meta;
  {
    std::shared_ptr<std::istream> is = trajectory::openStateFileReading(f);
    meta = trajectory::readMeta(is.get());
  }

  throw_rank(meta.rank);
  throw_type(meta.typeID);

  std::shared_ptr<std::istream> is = trajectory::openStateFileReading(f);

  list result;
  result.append(meta);

  switch (meta.rank) {
    #define BOOST_PP_LOCAL_MACRO(n) \
      case n: \
        if(meta.typeID=="CArray") result.extend(doRead<CArray<n>,n>(is.get())); \
        if(meta.typeID=="DArray") result.extend(doRead<DArray<n>,n>(is.get()));  \
        break;
    #define BOOST_PP_LOCAL_LIMITS (1, PYTHON_MAX_RANK)
    #include BOOST_PP_LOCAL_ITERATE()
  }
  return result;
}

void write(str filename, const numeric::array &array, double time)
{
  std::string f = extract<std::string>(filename);
  std::shared_ptr<std::ostream> ofs = trajectory::openStateFileWriting(f,std::ios_base::trunc | std::ios_base::binary);

  const PyArrayObject * np_array = numeric_np(array);

  int r=PyArray_NDIM(np_array);
  throw_rank(r);
  switch (r) {
    #define BOOST_PP_LOCAL_MACRO(n)                 \
      case n:                                       \
        if(PyArray_TYPE(np_array)==NPY_DOUBLE)  \
          doWrite<DArray<n>,double,n>(ofs.get(),array,time);  \
        if(PyArray_TYPE(np_array)==NPY_CDOUBLE) \
          doWrite<CArray<n>,dcomp,n>(ofs.get(),array,time);   \
        break;
    #define BOOST_PP_LOCAL_LIMITS (1, PYTHON_MAX_RANK)
    #include BOOST_PP_LOCAL_ITERATE()
  }
}


void pyDimensionsMismatchException(const trajectory::DimensionsMismatchException &)
{
  PyErr_SetString(PyExc_RuntimeError, "dimensions mismatch");
}

void pyRankMismatchException(const trajectory::RankMismatchException &)
{
  PyErr_SetString(PyExc_RuntimeError, "rank mismatch");
}

void pyStateFileOpeningException(const trajectory::StateFileOpeningException &e)
{
  PyErr_SetString(PyExc_IOError, (std::string("Could not open ")+e.getTag()).c_str());
}

void export_io()
{
  import_array1();
  numeric::array::set_module_and_type("numpy", "ndarray");
  def("read", read,
R"doc(Read in a state vector file.

:param str fname: The input filename.
:returns: A tuple of the form :samp:`(meta, states, times)`.)doc",
  arg("fname")
  );

  def("write", write,
R"doc(Write a state vector file.

:param str fname: The output filename.
:param ndarray a: The array to write.
:param double t: The time.)doc",
  (arg("fname"),"a", "t")
  );

  register_exception_translator<trajectory::DimensionsMismatchException>(&pyDimensionsMismatchException);
  register_exception_translator<trajectory::RankMismatchException>      (&pyRankMismatchException);
  register_exception_translator<trajectory::StateFileOpeningException>  (&pyStateFileOpeningException);

  class_<trajectory::SerializationMetadata>("SerializationMetadata")
    .def_readonly("protocolVersion", &trajectory::SerializationMetadata::protocolVersion)
    .def_readonly("rank",            &trajectory::SerializationMetadata::rank)
    .def_readonly("trajectoryID",    &trajectory::SerializationMetadata::trajectoryID);
}

} // pythonext
