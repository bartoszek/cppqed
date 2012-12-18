// -*- C++ -*-
/*

  Evolved is intended as a common interface for (adaptive stepsize) ODE drivers. It takes the Array it operates on as template parameter. A given Array can be adapted to the form expected by Evolved by a suitable specialization of ArrayMemoryTraits.

  The Array which is actually "Evolved" is by no means owned by Evolved (meaning that it is not deallocated when an Evolved is destructed).

  Note that the function calculating the derivative cannot be processed as metadata: Indeed, usually it's a functor, which incorporates additional data obtained at run-time.

*/

#ifndef UTILS_INCLUDE_EVOLVED_H_INCLUDED
#define UTILS_INCLUDE_EVOLVED_H_INCLUDED

#include "EvolvedFwd.h"

#include "ArrayTraitsFwd.h"

#include <boost/shared_ptr.hpp> // instead of std::tr1::shared_ptr
#include <boost/function.hpp>   // instead of std::tr1::function
#include <boost/utility.hpp>

#ifndef DO_NOT_USE_BOOST_SERIALIZATION
#include <boost/serialization/base_object.hpp>
#endif // DO_NOT_USE_BOOST_SERIALIZATION


namespace evolved {

  
class TimeStepBookkeeper
{
public:
  double getTime(        ) const {return t_;}
  void   setTime(double t)       {t_=t;}

  double getDtDid() const {return dtDid_;}

  double getDtTry(            ) const {return dtTry_;}
  void   setDtTry(double dtTry)       {dtTry_=dtTry;}

  double getEpsRel() const {return epsRel_;}
  double getEpsAbs() const {return epsAbs_;}

  void update(double t, double dtTry);
  void setDtDid(double dtDid) {dtDid_=dtDid;}

  TimeStepBookkeeper& operator=(const TimeStepBookkeeper&);

protected:
  TimeStepBookkeeper(double dtInit, double epsRel, double epsAbs);

private:
#ifndef DO_NOT_USE_BOOST_SERIALIZATION
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int) {ar & t_ & dtDid_ & dtTry_;}
#endif // DO_NOT_USE_BOOST_SERIALIZATION

  double t_, dtTry_, dtDid_;

  const double epsRel_, epsAbs_;

};


////////////////////
//
// Evolved interface
//
////////////////////

template<typename A>
class Evolved : public TimeStepBookkeeper, private boost::noncopyable 
{
public:
  typedef cpputils::ArrayMemoryTraits<A> Traits;

  typedef boost::function<void(double, const A&, A&)> Derivs;

  typedef boost::shared_ptr<Evolved> Ptr;

  Evolved(A&, Derivs, double dtInit, double epsRel, double epsAbs);

  using TimeStepBookkeeper::operator=;

  virtual ~Evolved() {}

  // Takes a single adaptive step of maximum length deltaT    
  void step(double deltaT);

  std::ostream& displayParameters(std::ostream& os) const {return doDisplayParameters(os);};

  A      & getA()       {return a_;}
  A const& getA() const {return a_;}

  const Derivs getDerivs() const {return derivs_;}

  // Number of failed steps in the last timestep
  size_t nFailedSteps() const {return reportNFailedSteps();}

private:
  virtual void doStep(double deltaT) = 0;
  virtual std::ostream& doDisplayParameters(std::ostream&) const = 0;
  virtual size_t reportNFailedSteps() const = 0;

#ifndef DO_NOT_USE_BOOST_SERIALIZATION
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int) {ar & boost::serialization::base_object<TimeStepBookkeeper>(*this) & a_;}
#endif // DO_NOT_USE_BOOST_SERIALIZATION

  A& a_;

  Derivs derivs_;

};



template<typename E>
void evolve(E&, double deltaT);
// evolves for exactly deltaT


template<typename E>
void evolveTo(E& e, double t)
// evolves to a given time t
{
  evolve(e,t-e.getTime());
}



////////////////
//
// factory class
//
////////////////

template<typename A>
class Maker
{
public:
  typedef typename Evolved<A>::Ptr Ptr;
  typedef typename Evolved<A>::Derivs   Derivs  ;
  
  virtual const Ptr operator()(A&, Derivs, double dtInit, double epsRel, double epsAbs, const A& scaleAbs) const = 0;

  virtual ~Maker() {}
  
};



} // evolved


#endif // UTILS_INCLUDE_EVOLVED_H_INCLUDED
