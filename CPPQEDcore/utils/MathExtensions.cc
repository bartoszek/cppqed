#include "MathExtensions.h"

#include <boost/math/special_functions/factorials.hpp>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>

namespace mathutils {
  
const double PI(M_PI);
const double SQRTPI(M_SQRTPI);
const double EULER(M_E);

int sign(double x) {return GSL_SIGN(x);}
int fcmp(double x, double y, double eps) {return gsl_fcmp(x,y,eps);}

double sqr(double x) {return gsl_pow_2(x);}

double sqrAbs(const dcomp& x) {return sqr(real(x))+sqr(imag(x));} // saves the sqrt

double fact(unsigned n) throw(FactOverflow)
{
  if (n>GSL_SF_FACT_NMAX) throw FactOverflow();
  return gsl_sf_fact(n);
}

double choose(unsigned n, unsigned m)
{
  return gsl_sf_choose(n,m);
}

bool parity(         long n) {return GSL_IS_ODD(n);}
bool parity(unsigned long n) {return GSL_IS_ODD(n);}


dcomp coherentElement(unsigned long n, const dcomp& alpha)
{
  using namespace boost::math;
  return n ? n<max_factorial<long double>::value ? pow(alpha,n)/sqrt(factorial<long double>(n)) 
                                                 : pow(2*n*PI,-.25)*pow(alpha/sqrt(n/EULER),n)
           : 1.;
}


} // mathutils