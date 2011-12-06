#include "EvolvedGSL.h"

#include<gsl/gsl_errno.h>
#include<gsl/gsl_odeiv.h>


namespace evolved {


namespace details  {


// The purpose of the arrangement is to separate the visible interface from GSL. (That is, no GSL headers need be included in EvolvedGSL.h)


class Impl
{
public:
  Impl(void*, size_t, int(double,const double*,double*,void*), double, double, const double*, SteppingFunction);

  ~Impl();

private:
  // NEEDS_WORK See which of the following can be made const
  gsl_odeiv_system dydt_;
  gsl_odeiv_step   *const s_;
  gsl_odeiv_control*const c_;
  gsl_odeiv_evolve *const e_;

  friend void apply(ImplSmartPtr,double*,double,double*,double*);
  friend size_t extractFailedSteps(ImplSmartPtr);

};


gsl_odeiv_system GSL_OdeivSystemFill(void* self, size_t size, int (*derivs)(double, const double*, double*, void*))
{
  gsl_odeiv_system res;
  res.function=derivs;
  res.jacobian=0; 
  res.dimension=size;
  res.params=self;
  return res;
}


Impl::Impl(void* self, size_t size, int (*derivs)(double, const double*, double*, void*), double epsRel, double epsAbs, const double* scaleAbs, SteppingFunction sf)
  : dydt_(GSL_OdeivSystemFill(self,size,derivs)),
    s_(gsl_odeiv_step_alloc(sf==SF_RKCK ? gsl_odeiv_step_rkck : gsl_odeiv_step_rk8pd,size)),
    c_(
       scaleAbs
       ?
       gsl_odeiv_control_scaled_new  (epsAbs,epsRel,1,1,scaleAbs,size)
       :
       gsl_odeiv_control_standard_new(epsAbs,epsRel,1,1)
       ),
    e_(gsl_odeiv_evolve_alloc(size))
{}


Impl::~Impl()
{
  gsl_odeiv_evolve_free(e_);
  gsl_odeiv_control_free(c_);
  gsl_odeiv_step_free(s_);
}


ImplSmartPtr createImpl(void* self, size_t size, int (*derivs)(double, const double*, double*, void*),
			double epsRel, double epsAbs, const double* scaleAbs, SteppingFunction sf)
{
  return ImplSmartPtr(new Impl(self,size,derivs,epsRel,epsAbs,scaleAbs,sf));
}


void apply(ImplSmartPtr p, double* t, double t1, double* h, double* y)
{
  gsl_odeiv_evolve_reset(p->e_);
  gsl_odeiv_evolve_apply(p->e_,p->c_,p->s_,&p->dydt_,t,t1,h,y);
}


size_t extractFailedSteps(ImplSmartPtr p)
{
  return p->e_->failed_steps;
}


const int onSuccess=GSL_SUCCESS;


} // details


using namespace std;

ostream& operator<<(ostream& os, SteppingFunction sf)
{
  switch (sf) {
  case SF_RKCK  : return os<<"rkck" ;
  case SF_RK8PD :        os<<"rk8pd";
  }
  return os;
}

istream& operator>>(istream& is, SteppingFunction& sf) 
{
  SteppingFunction sftemp=SF_RK8PD;
  string s;

  is>>s;
       if (s=="rkck"  ) sftemp=SF_RKCK ;
  else if (s!="rk8pd") 
    is.clear(ios_base::badbit);

  if (is) sf=sftemp;
  return is;
}


} // evolved
