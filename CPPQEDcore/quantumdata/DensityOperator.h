// Copyright András Vukics 2006–2020. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt)
/// \briefFileDefault
#ifndef CPPQEDCORE_QUANTUMDATA_DENSITYOPERATOR_H_INCLUDED
#define CPPQEDCORE_QUANTUMDATA_DENSITYOPERATOR_H_INCLUDED

#include "QuantumDataFwd.h"

#include "ArrayBase.h"
#include "DimensionsBookkeeper.h"
#include "LazyDensityOperator.h"
#include "Types.h"

#include "BlitzArrayExtensions.tcc"
#include "MultiIndexIterator.h"


namespace quantumdata {


/// Density operator of arbitrary arity
/**
 * Cf. \ref quantumdatahighlevel "rationale"
 * 
 * \tparamRANK
 * 
 * The DensityOperator interface is similar to StateVector with obvious differences.
 * 
 * \note A DensityOperator `<RANK>` represents a density operator on a Hilbert space of arity `RANK`. This makes that the number of its indices is actually `2*RANK`.
 * 
 */
template<int RANK>
class DensityOperator
  : public LazyDensityOperator<RANK>,
    public ArrayBase<DensityOperator<RANK>>
{
public:
  static const int N_RANK=RANK;
  
  typedef LazyDensityOperator<  RANK> LDO_Base;
  
  typedef ArrayBase<DensityOperator<RANK>> ABase;

  typedef typename LDO_Base::Dimensions Dimensions;

  typedef typename LDO_Base::Idx Idx;

  using DensityOperatorLow=typename ABase::ArrayLow;

  typedef linalg::CMatrix CMatrix;

  using ABase::frobeniusNorm; using ABase::getArray; using ABase::operator=;

  /*
  DensityOperator() : Base() {}
  DensityOperatorBase() : LDO_Base(Dimensions()), ABase(DensityOperatorLow()), matrixView_() {}
  */

  DensityOperator(const DensityOperatorLow& rho, ByReference) ///< Referencing constructor implemented in terms of blitzplusplus::halfCutTiny
    : LDO_Base(blitzplusplus::halfCutTiny(rho.shape())), ABase(rho) {}

  explicit DensityOperator(const Dimensions& dimensions, bool init=true)
    : LDO_Base(dimensions),
      ABase(DensityOperatorLow(blitzplusplus::concatenateTinies(dimensions,dimensions))) {if (init) *this=0;}

  explicit DensityOperator(const StateVector<RANK>& psi) ///< Constructs the class as a dyadic product of `psi` with itself.
    : LDO_Base(psi.getDimensions()), ABase(psi.dyad()) {}

  DensityOperator(const DensityOperator& rho) ///< By-value copy constructor (deep copy)
    : LDO_Base(rho.getDimensions()), ABase(rho.getArray().copy()) {}


  DensityOperator(DensityOperator&& rho) ///< Move constructor (shallow copy)
    : LDO_Base(rho.getDimensions()), ABase(rho.getArray()) {}

  /// Default assignment doesn't work, because LazyDensityOperator is always purely constant (const DimensionsBookkeeper base)
  DensityOperator& operator=(const DensityOperator& rho) {ABase::operator=(rho.getArray()); return *this;}

private:
  class IndexerProxy
  {
  public:
    IndexerProxy(const DensityOperator* rho, const Idx& firstIndex) : rho_(rho), firstIndex_(firstIndex) {}

    template<typename... SubscriptPack>
    IndexerProxy(const DensityOperator* rho, int s0, SubscriptPack... subscriptPack) : IndexerProxy(rho,Idx(s0,subscriptPack...))
    {static_assert( sizeof...(SubscriptPack)==RANK-1 , "Incorrect number of subscripts for DensityOperator." );}

    const dcomp& operator()(const Idx& secondIndex) const {return rho_->indexWithTiny(firstIndex_,secondIndex);}
          dcomp& operator()(const Idx& secondIndex)       {return const_cast<dcomp&>(static_cast<const IndexerProxy&>(*this)(secondIndex));}

    template<typename... SubscriptPack>
    const dcomp& operator()(int s0, SubscriptPack... subscriptPack) const
    {
      static_assert( sizeof...(SubscriptPack)==RANK-1 , "Incorrect number of subscripts for DensityOperator." );
      return operator()(Idx(s0,subscriptPack...));
    }

    template<typename... SubscriptPack>
          dcomp& operator()(int s0, SubscriptPack... subscriptPack) {return const_cast<dcomp&>(static_cast<const IndexerProxy&>(*this)(s0,subscriptPack...));}

  private:
    const DensityOperator*const rho_;
    const Idx firstIndex_;

  };

  friend class IndexerProxy;

public:
  /// \name (Multi-)matrix style indexing
  //@{
  const IndexerProxy operator()(const Idx& firstIndex) const {return IndexerProxy(this,firstIndex);}
        IndexerProxy operator()(const Idx& firstIndex)       {return IndexerProxy(this,firstIndex);}

  template<typename... SubscriptPack>
  const IndexerProxy operator()(int s0, SubscriptPack... subscriptPack) const {return IndexerProxy(this,s0,subscriptPack...);}

  template<typename... SubscriptPack>
        IndexerProxy operator()(int s0, SubscriptPack... subscriptPack)       {return IndexerProxy(this,s0,subscriptPack...);}
  //@}

  /// \name LazyDensityOperator diagonal iteration
  //@{
  template<typename... SubscriptPack>
  auto diagonalSliceIndex(SubscriptPack&&... subscriptPack) const
  {
    static_assert( sizeof...(SubscriptPack)==RANK , "Incorrect number of subscripts for DensityOperator." );
#define SLICE_EXPR getArray()(std::forward<SubscriptPack>(subscriptPack)...,std::forward<SubscriptPack>(subscriptPack)...)
    return DensityOperator<cpputils::Rank<decltype(SLICE_EXPR)>::value/2>(SLICE_EXPR,byReference);
#undef  SLICE_EXPR
  }
  
  template<typename... SubscriptPack>
  void transposeSelf(SubscriptPack... subscriptPack)
  {
    static_assert( sizeof...(SubscriptPack)==RANK , "Incorrect number of subscripts for StateVector." );
    getArray().transposeSelf(subscriptPack...,(subscriptPack+RANK)...);
  }
  //@}
  
  /// \name Norm
  //@{
  double norm() const ///< returns the trace norm
  {
    using blitz::tensor::i;
    const linalg::CMatrix m(matrixView());
    return real(sum(m(i,i)));
  }

  double renorm() ///< ” and also renormalises
  {
    double trace=norm();
    *this/=trace;
    return trace;
  }
  //@}

  /// \name Matrix view
  //@{
  auto matrixView() const {return blitzplusplus::binaryArray(getArray());} ///< returns a two-dimensional view of the underlying data, created on the fly via blitzplusplus::binaryArray
  //@}
  
private:
  const dcomp& indexWithTiny(const Idx& i, const Idx& j) const ///< Used for implementing operator() and the index function below.
  {
    return getArray()(blitzplusplus::concatenateTinies<int,int,RANK,RANK>(i,j));
  }
  
  const dcomp index(const Idx& i, const Idx& j) const override {return indexWithTiny(i,j);} ///< This function implements the LazyDensityOperator interface in a trivial element-access way

  double trace_v() const override {return norm();} ///< A straightforward implementation of a LazyDensityOperator virtual

};


template<int RANK1, int RANK2>
inline
const DensityOperator<RANK1+RANK2>
operator*(const DensityOperator<RANK1>&, const DensityOperator<RANK2>&);


template<int RANK>
inline
double frobeniusNorm(const DensityOperator<RANK>& rho) {return rho.frobeniusNorm();}


/// Performs the opposite of quantumdata::deflate
template<int RANK>
void inflate(const DArray<1>& flattened, DensityOperator<RANK>& rho, bool offDiagonals)
{
  using mathutils::sqr;

  const size_t dim=rho.getTotalDimension();
  
  typedef cpputils::MultiIndexIterator<RANK> Iterator;
  const Iterator etalon(rho.getDimensions()-1,cpputils::mii::begin);
  
  size_t idx=0;

  // Diagonal
  for (Iterator i(etalon); idx<dim; ++i)
    rho(*i)(*i)=flattened(idx++);
  
  // OffDiagonal
  if (offDiagonals)
    for (Iterator i(etalon); idx<mathutils::sqr(dim); ++i)
      for (Iterator j=++Iterator(i); j!=etalon.getEnd(); ++j, idx+=2) {
        dcomp matrixElement(rho(*i)(*j)=dcomp(flattened(idx),flattened(idx+1)));
        rho(*j)(*i)=conj(matrixElement);
      }

}


/// Creates a DensityOperator as the (deep) copy of the data of a LazyDensityOperator of the same arity
template<int RANK>
const DensityOperator<RANK>
densityOperatorize(const LazyDensityOperator<RANK>& matrix)
{
  DensityOperator<RANK> res(matrix.getDimension());
  
  typedef cpputils::MultiIndexIterator<RANK> Iterator;
  const Iterator etalon(matrix.getDimensions()-1,cpputils::mii::begin);
  
  for (Iterator i(etalon); i!=etalon.getEnd(); ++i) {
    res(*i)(*i)=matrix(*i);
    for (Iterator j=++Iterator(i); j!=etalon.getEnd(); ++j) {
      dcomp matrixElement(res(*i)(*j)=matrix(*i)(*j));
      res(*j)(*i)=conj(matrixElement);
    }
  }

  return res;
  
}


template<int... SUBSYSTEM, int RANK>
const DensityOperator<mpl::size<tmptools::Vector<SUBSYSTEM...> >::value>
reduce(const LazyDensityOperator<RANK>& matrix)
{
  static const int RES_ARITY=mpl::size<tmptools::Vector<SUBSYSTEM...> >::value;
  return partialTrace<tmptools::Vector<SUBSYSTEM...>,DensityOperator<RES_ARITY> >(matrix,densityOperatorize<RES_ARITY>);
}


template<int RANK>
inline auto
dyad(const StateVector<RANK>& sv1, const StateVector<RANK>& sv2)
{
  return DensityOperator<RANK>(sv1.dyad(sv2),byReference);
}


template <int RANK> struct ArrayRank<DensityOperator<RANK>> {static const int value=2*RANK;};

} // quantumdata


#endif // CPPQEDCORE_QUANTUMDATA_DENSITYOPERATOR_H_INCLUDED
