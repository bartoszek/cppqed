// -*- C++ -*-
#ifndef   _BLITZ_TO_FLENS_IMPL_H
#define   _BLITZ_TO_FLENS_IMPL_H

#include "Blitz2FLENS.h"

#include "TMP_Tools.h"

#include "impl/BlitzTinyExtensions.tcc"
#include "ComplexArrayExtensions.h"


namespace blitz2flens {


template<typename T, int RANK>
const typename DenseVectorMF<T>::type vector(blitz::Array<T,RANK>& array)
{
  typedef typename DenseVectorMF<T>::type DenseVector;
  typedef typename DenseVectorMF<T>::View ArrayView  ;

  assert(array.isStorageContiguous());
  // NEEDS_WORK more assertions are needed to ensure that the data
  // will indeed be interpreted in the correct way by FLENS

  return DenseVector(ArrayView(array.dataFirst(),array.dataFirst(),array.size(),1,0));
}


template<typename T, int TWO_TIMES_RANK, StorageOrder SO>
const typename GeMatrixMF<T,SO>::type matrix(blitz::Array<T,TWO_TIMES_RANK>& array, StorageTag<SO> =StorageTag<SO>())
{
  /*static const int RANK=*/tmptools::IsEvenAssert<TWO_TIMES_RANK>();//::value;

  using namespace flens;
  typedef typename GeMatrixMF<T,SO>::type GeMatrix       ;
  typedef typename GeMatrixMF<T,SO>::View FullStorageView;

  assert(!array.size() || array.isStorageContiguous());
  // NEEDS_WORK assert(SO==RowMajor && ) ordering!!!

  size_t dim=product(blitzplusplus::halfCutTiny(array.extent()));
  // halfCutTiny will throw if the dimensions are not the same

  return GeMatrix(FullStorageView(array.dataFirst(),array.dataFirst(),dim,dim,dim,0,0));

}


template<int TWO_TIMES_RANK, StorageOrder SO>
const typename HeMatrixMF<SO>::type hermitianMatrix(TTD_CARRAY(TWO_TIMES_RANK)& array, StorageTag<SO> =StorageTag<SO>())
{
  /*static const int RANK=*/tmptools::IsEvenAssert<TWO_TIMES_RANK>();//::value;

  using namespace flens;
  using namespace blitzplusplus;

  typedef typename HeMatrixMF<SO>::type HeMatrix       ;
  typedef typename HeMatrixMF<SO>::View FullStorageView;

  assert(!array.size() || array.isStorageContiguous());
  // NEEDS_WORK assert(SO==RowMajor && ) ordering!!!

  size_t dim=product(halfCutTiny(array.extent()));
  // halfCutTiny will throw if the dimensions are not the same

  return HeMatrix(FullStorageView(array.dataFirst(),array.dataFirst(),dim,dim,dim,0,0),Upper);

}


} // blitz2flens


#endif // _BLITZ_TO_FLENS_IMPL_H
