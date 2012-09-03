// -*- C++ -*-
#ifndef   ELEMENTS_COMPOSITES_COMPOSITE_H_INCLUDED
#define   ELEMENTS_COMPOSITES_COMPOSITE_H_INCLUDED

#include "CompositeFwd.h"

#include "Act.h"

#include "BlitzArraySliceIterator.h"
// This is included at this point mainly to pull in necessary TMP tools

#include "details/TMP_helpers.h"

#include <boost/fusion/container/generation/make_list.hpp>


/*
  TODO:
  * in user guide:
    - document changes of BinarySystem (binary::make)
    - composite::make
    - composite::result_of::Make
    - element makers => make
*/


namespace composite {

using boost::fusion::make_list;

namespace result_of {

using boost::fusion::result_of::make_list;

} // result_of


template<typename VA>
struct MaxRank : MaxMF<typename MaxMF<VA,composite::SeqLess<mpl::_1,mpl::_2> >::type>::type::type {};


template<typename VA>
class Exact
  : public structure::Exact<MaxRank<VA>::value+1>
{
private:
  static const int RANK=composite::MaxRank<VA>::value+1;

  typedef blitz::TinyVector<structure::SubSystemFree,RANK> Frees;

  typedef quantumdata::Types<RANK> Types;
  typedef typename Types::StateVectorLow StateVectorLow;

  typedef tmptools::Ordinals<RANK> Ordinals;

protected:
  Exact(const Frees& frees, const VA& acts) : frees_(frees), acts_(acts) {}

private:
  bool isUnitary(                       ) const; class IsUnitary;
  void actWithU (double, StateVectorLow&) const; class ActWithU ;

  const Frees& frees_;
  const VA   &  acts_;

};


} // composite



template<typename VA>
// VA should model a fusion sequence of Acts
class Composite 
// The base_from_member idiom appears because the Frees has to be calculated and stored somehow first
  : private boost::base_from_member<const blitz::TinyVector<structure::SubSystemFree,composite::MaxRank<VA>::value+1> >,
    public structure::QuantumSystem<composite::MaxRank<VA>::value+1>,
    public composite::Exact<VA>, 
    public structure::Hamiltonian  <composite::MaxRank<VA>::value+1>,
    public structure::Liouvillean  <composite::MaxRank<VA>::value+1>,
    public structure::Averaged     <composite::MaxRank<VA>::value+1>
{
public:
  // The calculated RANK
  static const int RANK=composite::MaxRank<VA>::value+1;

  // Public types

  typedef structure::QuantumSystem<RANK> QS_Base;
  typedef structure::Hamiltonian  <RANK> Ha_Base;
  typedef structure::Liouvillean  <RANK> Li_Base;
  typedef structure::Averaged     <RANK> Av_Base;

  typedef blitz::TinyVector<structure::SubSystemFree,RANK> Frees;

  typedef boost::base_from_member<const Frees> FreesBase;

  typedef quantumdata::Types<RANK> Types;

  typedef typename Types::    StateVectorLow     StateVectorLow;
  typedef typename Types::DensityOperatorLow DensityOperatorLow;

  typedef quantumdata::LazyDensityOperator<RANK> LazyDensityOperator;

  typedef tmptools::Ordinals<RANK> Ordinals;

  typedef typename QS_Base::Dimensions    Dimensions   ;
  typedef typename Li_Base::Probabilities Probabilities;
  typedef typename Av_Base::Averages      Averages     ;

  // Base class names

  using QS_Base::getDimensions; using QS_Base::getTotalDimension; using Li_Base::defaultArray;

  // Compile-time sanity check

  BOOST_MPL_ASSERT_MSG( ( composite::CheckMeta<RANK,VA>::type::value == true ), COMPOSITE_not_CONSISTENT, (mpl::void_) );

private:
  // Constructor helpers
		      
  static const Frees fillFrees(const VA&);

  static const Dimensions fillDimensions(const Frees&);

public:
  // Constructor

  explicit Composite(const VA& acts) 
    : FreesBase(fillFrees(acts)), QS_Base(fillDimensions(FreesBase::member)), composite::Exact<VA>(FreesBase::member,acts), frees_(FreesBase::member), acts_(acts) {}


private:
  // Implementing QS_Base

  double highestFrequency (             ) const;

  void   displayParameters(std::ostream&) const; class DisplayParameters;

  // Implementing Ha_Base

  void addContribution(double, const StateVectorLow&, StateVectorLow&, double) const; class Hamiltonian;

  // Implementing Li_Base

  size_t              nJumps       ()                                   const; class NJumps;
  const Probabilities probabilities(double, const LazyDensityOperator&) const; class Probas;
  void                actWithJ     (double, StateVectorLow&, size_t)    const; class ActWithJ;

  // Implementing Av_Base

  void   displayKey(std::ostream&, size_t&) const; class DisplayKey;
  size_t nAvr      ()                       const; class NAvr;

  const Averages average(double, const LazyDensityOperator&)  const; class Average;
  void           process(Averages&)                           const; class Process;
  void           display(const Averages&, std::ostream&, int) const; class Display;


  // Storage
  // Note that Frees are stored by value in FreesBase

  const Frees& frees_;
  const VA      acts_;

};


// The following provides a much more convenient interface:

namespace composite {

namespace result_of {


namespace mpl=boost::mpl;

typedef Act<> DefaultArgument;


template<BOOST_PP_ENUM_BINARY_PARAMS(FUSION_MAX_VECTOR_SIZE,typename A,=DefaultArgument BOOST_PP_INTERCEPT)> 
struct Make : boost::mpl::identity<Composite<typename make_list<BOOST_PP_ENUM_PARAMS(FUSION_MAX_VECTOR_SIZE,A)>::type> >
{};


} // result_of

} // composite


#define DEFAULT_print(z, n, data) DefaultArgument

#define BOOST_PP_ITERATION_LIMITS (1,BOOST_PP_SUB(FUSION_MAX_VECTOR_SIZE,1) )
#define BOOST_PP_FILENAME_1 "details/CompositeMakerImplementationsSpecialization.h"

#include BOOST_PP_ITERATE()

#undef BOOST_PP_FILENAME_1
#undef BOOST_PP_ITERATION_LIMITS

#undef DEFAULT_print


#endif // ELEMENTS_COMPOSITES_COMPOSITE_H_INCLUDED
