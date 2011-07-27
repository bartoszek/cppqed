// -*- C++ -*-
/*
  Wrapper functions for mathematical functions taken from libraries,
  and several other mathematical functions.

  The principal aim of Math.h and the corresponding Math.cc is to
  localize dependence on GSL.

*/

#ifndef _CPPUTILS_MATH_H
#define _CPPUTILS_MATH_H

#include "MathExtensionsFwd.h"

#include "ComplexExtensions.h"
#include "Exception.h"


namespace mathutils {

struct FactOverflow  : public cpputils::Exception {};
  
extern const double PI    ;
extern const double SQRTPI;
  
int sign(double);
int fcmp(double, double, double);
  
template<class T> const T sqr(T x) {return x*x;}

double sqr(double x);

double sqrAbs(const dcomp&);

double fact  (unsigned          ) throw(FactOverflow);
double choose(unsigned, unsigned);

bool parity(         long);
bool parity(unsigned long);
// even --- false, odd --- true;


} // mathutils

#endif // _CPPUTILS_MATH_H