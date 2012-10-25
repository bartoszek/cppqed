// -*- C++ -*-
#ifndef ELEMENTS_INTERACTIONS_JAYNESCUMMINGS__H_INCLUDED
#define ELEMENTS_INTERACTIONS_JAYNESCUMMINGS__H_INCLUDED

// #include<boost/type_traits/is_base_of.hpp>

#include "JaynesCummingsFwd.h"

#include "Interaction.h"
#include "TridiagonalHamiltonian.h"

#include "Mode_.h"

#include "ParsFwd.h"


namespace jaynescummings {


struct Pars
{
  dcomp& g;

  operator dcomp&() const {return g;}

  Pars(parameters::ParameterTable&, const std::string& ="");

};


typedef boost::shared_ptr<const structure::Free> QbitSpinCommonPtr;

const structure::free::Tridiagonal sigmaop(QbitSpinCommonPtr); // a simple dispatcher

class UnrecognizedPluginToJaynesCummings : public cpputils::Exception {};


class Base : public structure::Interaction<2>, public structure::TridiagonalHamiltonian<2,true>
{
protected:
  typedef structure::Interaction<2> IA_Base;
  typedef structure::TridiagonalHamiltonian<2,true> TDH_Base;

  Base(QbitSpinCommonPtr, mode::Ptr, const dcomp& g);

};


} // jaynescummings


#define BIG_NAMESPACE_NAME             jaynescummings
#define BIG_CLASS_NAME                 JaynesCummings
#define BIG_ADDITIONAL_PARAMETERS      , const dcomp& g
#define BIG_ADDITIONAL_PARAMETERS_PASS ,g

#include "details/BinaryInteractionGenerator.h"


/*
namespace details {

template<typename QbitType, typename ModeType>
struct IsTimeDependent : mpl::bool_<boost::is_base_of<qbit::Exact,QbitType>::value
				    ||
				    boost::is_base_of<mode::Exact,ModeType>::value
				    > {};

} // details


template<typename QbitType, typename ModeType>
class JaynesCummings 
  : public JaynesCummingsBase, 
    public structure::TridiagonalHamiltonian<2,details::IsTimeDependent<QbitType,ModeType>::value>
{
public:
  static const bool IS_TD=details::IsTimeDependent<QbitType,ModeType>::value;
  typedef structure::TridiagonalHamiltonian<2,IS_TD> TDH_Base;

  JaynesCummings(const QbitType&, const ModeType&, const dcomp& g, mpl::bool_<IS_TD> =mpl:: true_());
  JaynesCummings(const QbitType&, const ModeType&, const dcomp& g, mpl::bool_<IS_TD> =mpl::false_());

};

*/
#endif // ELEMENTS_INTERACTIONS_JAYNESCUMMINGS__H_INCLUDED
