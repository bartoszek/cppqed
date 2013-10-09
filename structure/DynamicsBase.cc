#include "DynamicsBase.h"

#include "FormDouble.tcc"
#include "Range.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <boost/mpl/identity.hpp>

namespace mpl=boost::mpl;

using namespace std;

namespace structure {


DynamicsBase::DynamicsBase(const RealFreqs& realFreqs, const ComplexFreqs& complexFreqs) 
  : realFreqs_(realFreqs), complexFreqs_(complexFreqs), paramsStream_(std::stringstream::out)
{
  paramsStream_.precision(formdouble::actualPrecision(FormDouble::overallPrecision));
}



namespace {

template <typename T> using NamedFrequency=typename std::list<boost::tuple<std::string,T,double> >::value_type;

inline double absReal   (const NamedFrequency<double>& p) {return fabs(p.get<1>()*p.get<2>());}
inline double absComplex(const NamedFrequency<dcomp >& p) {return  abs(p.get<1>()*p.get<2>());}


template<typename T>
bool templateCompare(const NamedFrequency<T>& p1, const NamedFrequency<T>& p2,
                     boost::function<double(const NamedFrequency<T>&)> f)
{
  return f(p1)<f(p2);
}

} // unnamed namespace


double DynamicsBase::highestFrequency() const
{
  using boost::max_element;
  return
    max(
        realFreqs_.size() 
        ? 
        absReal   (*max_element(realFreqs_   ,bind(templateCompare<double>,_1,_2,absReal   )))
        :
        0,
        complexFreqs_.size()
        ?
        absComplex(*max_element(complexFreqs_,bind(templateCompare<dcomp >,_1,_2,absComplex)))
        :
        0
        );

}



std::ostream& DynamicsBase::displayParameters(ostream& os) const
{
  return displayMoreParameters(os<<paramsStream_.str())<<endl;
}


namespace {

template<typename T>
void displayFreq(ostream& os, int precision, const NamedFrequency<T>& pair)
{
  os<<"# "<<pair.template get<0>()<<"="<<formdouble::zeroWidth(precision)(pair.template get<1>())<<endl;
}

} // unnamed namespace


std::ostream& DynamicsBase::displayMoreParameters(ostream& os) const
{
  using boost::for_each;
  for_each(   realFreqs_,bind(displayFreq<double>,boost::ref(os),os.precision(),_1));
  for_each(complexFreqs_,bind(displayFreq<dcomp >,boost::ref(os),os.precision(),_1));
  return os;
}


} // structure

