// -*- C++ -*-
#ifndef ELEMENTS_COMPOSITES_BINARYSYSTEM_H_INCLUDED
#define ELEMENTS_COMPOSITES_BINARYSYSTEM_H_INCLUDED

#include "BinarySystemFwd.h"

#include "QuantumSystem.h"
#include "SubSystem.h"

#include "SmartPtr.h"


namespace binary {


typedef boost::shared_ptr<const Base> Ptr;

typedef structure::Interaction<2> Interaction;


const Ptr doMake(Interaction::Ptr);

template<typename IA>
const Ptr make(const IA& ia)
{
  return doMake(cpputils::sharedPointerize(ia));
}


typedef composite::SubSystemFree            SSF;
typedef composite::SubSystemsInteraction<2> SSI;



class Base
  : public structure::QuantumSystem<2>,
    public structure::Averaged     <2>
{
public:
  typedef structure::Averaged<1> Av1;
  typedef structure::Averaged<2> Av2;

  explicit Base(Interaction::Ptr);

  const SSF& getFree0() const {return free0_;}
  const SSF& getFree1() const {return free1_;}
  const SSI& getIA   () const {return    ia_;}

private:
  double  highestFrequency_v(             ) const;
  void   displayParameters_v(std::ostream&) const;

  size_t            nAvr_v()                                    const;
  const Averages average_v(double, const LazyDensityOperator&)  const;
  void           process_v(Averages&)                           const;
  void           display_v(const Averages&, std::ostream&, int) const;
  void        displayKey_v(std::ostream&, size_t&)              const;

  const SSF free0_, free1_;

  const SSI ia_;
  
};


#define CLASS_HEADER(Class) class Class : public structure::Class<2>

#define CLASS_BODY_PART(Class,Aux) public:				\
  typedef structure::Class<1> Aux##1;					\
  typedef structure::Class<2> Aux##2;					\
									\
  Class(const SSF& free0, const SSF& free1, const SSI& ia) : free0_(free0), free1_(free1), ia_(ia) {} \
									\
private:								\
  const SSF &free0_, &free1_;						\
  const SSI &ia_;							\



CLASS_HEADER(Exact)
{
  CLASS_BODY_PART(Exact,Ex)

  bool isUnitary_v() const;

  void  actWithU_v(double, StateVectorLow&) const;

};


CLASS_HEADER(Hamiltonian)
{
  CLASS_BODY_PART(Hamiltonian,Ha)

  void addContribution_v(double, const StateVectorLow&, StateVectorLow&, double) const;

};


CLASS_HEADER(Liouvillean)
{
  CLASS_BODY_PART(Liouvillean,Li)

  size_t                     nJumps_v()                                   const;
  const Probabilities probabilities_v(double, const LazyDensityOperator&) const;
  void                     actWithJ_v(double, StateVectorLow&, size_t)    const;

  void displayKey_v(std::ostream&, size_t&) const;

};


#undef CLASS_BODY_PART
#undef CLASS_HEADER


template<typename>
class EmptyBase
{
public:
  EmptyBase(const SSF&, const SSF&, const SSI&) {}
};


} // binary


#define BASE_CLASS(Aux,Class) mpl::if_c<IS_##Aux,binary::Class,binary::EmptyBase<binary::Class> >::type


template<bool IS_EX, bool IS_HA, bool IS_LI>
class BinarySystem
  : public binary::Base,
    public BASE_CLASS(EX,Exact), 
    public BASE_CLASS(HA,Hamiltonian),
    public BASE_CLASS(LI,Liouvillean)
{
public:
  typedef typename BASE_CLASS(EX,Exact)             ExactBase;
  typedef typename BASE_CLASS(HA,Hamiltonian) HamiltonianBase;
  typedef typename BASE_CLASS(LI,Liouvillean) LiouvilleanBase;
  
  typedef structure::Interaction<2> Interaction;

  explicit BinarySystem(Interaction::Ptr);

};


#undef BASE_CLASS


#endif // ELEMENTS_COMPOSITES_BINARYSYSTEM_H_INCLUDED
