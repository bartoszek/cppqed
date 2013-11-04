#include "Mode.tcc"

#include "ParsMode.h"

#include "StateVector.tcc"

#include "TridiagonalHamiltonian.tcc"

#include <boost/assign/list_inserter.hpp>


using std::cout; using std::endl; using std::string;
using namespace boost;
using namespace mathutils;


namespace mode {

#define DEFINE_make_by_redirect(AUX) const Ptr make(const BOOST_PP_CAT(Pars,AUX) & p, QM_Picture qmp) {return make<Averaged>(p,qmp);}

DEFINE_make_by_redirect()
DEFINE_make_by_redirect(Lossy)
DEFINE_make_by_redirect(Pumped)
DEFINE_make_by_redirect(PumpedLossy)

#undef DEFINE_make_by_redirect


const Tridiagonal aop(size_t);


////////
//
// Exact
//
////////


Exact::Exact(const dcomp& zI, size_t dim)
  : FreeExact(dim), zI_(zI)
{
}


void Exact::updateU(Time t) const
{
  getDiagonal()=exp(-zI_*(double(t)*blitz::tensor::i));
}


//////////////
//
// Hamiltonian
//
//////////////


const Tridiagonal::Diagonal mainDiagonal(const dcomp& z, size_t dim)
{
  Tridiagonal::Diagonal res(dim);
  res=blitz::tensor::i;
  return res*=z;
}


const Tridiagonal pumping(const dcomp& eta, size_t dim)
{
  return tridiagMinusHC(eta*aop(dim).dagger());
}


namespace {


const Tridiagonal hOverI(const dcomp& z, const dcomp& eta, size_t dim)
// Here we use runtime dispatching because runtime dispatching will
// anyway happen at the latest in some maker function.
{
  bool isPumped=isNonZero(eta);
  if (isNonZero(z)) {
    Tridiagonal res(mainDiagonal(-z,dim));
    if (isPumped) return res+pumping(eta,dim);
    else return res;
  }
  else if(isPumped) return pumping(eta,dim);
  else return Tridiagonal();
}


}


template<>
Hamiltonian<true >::Hamiltonian(const dcomp& zSch, const dcomp& zI, const dcomp& eta, size_t dim, boost::mpl::true_)
  : Base(furnishWithFreqs(hOverI(zSch,eta,dim),mainDiagonal(zI,dim))), Exact(zI,dim), zSch_(zSch), eta_(eta), dim_(dim)
{}

template<>
Hamiltonian<false>::Hamiltonian(const dcomp& zSch, const dcomp& eta, size_t dim, boost::mpl::false_)
  : Base(hOverI(zSch,eta,dim)), zSch_(zSch), eta_(eta), dim_(dim)
{}




//////////////
//
// Liouvillean
//
//////////////

namespace {


void aJump   (StateVectorLow& psi, double kappa)
{
  double fact=sqrt(2.*kappa);
  int ubound=psi.ubound(0);
  for (int n=0; n<ubound; ++n)
    psi(n)=fact*sqrt(n+1)*psi(n+1);
  psi(ubound)=0;
}


double aJumpRate   (const LazyDensityOperator& matrix, double kappa)
{
  return 2.*kappa*photonNumber(matrix);
}


}


Liouvillean<true >::Liouvillean(double kappa, double nTh, const std::string& kT)
  : Base(kT,{"excitation loss","excitation absorption"}), kappa_(kappa), nTh_(nTh)
{
}


void Liouvillean<true>::doActWithJ(NoTime, StateVectorLow& psi, JumpNo<0>) const
{
  aJump(psi,kappa_*(nTh_+1));
}


void Liouvillean<true>::doActWithJ(NoTime, StateVectorLow& psi, JumpNo<1>) const
{
  double fact=sqrt(2.*kappa_*nTh_);
  for (int n=psi.ubound(0); n>0; --n)
    psi(n)=fact*sqrt(n)*psi(n-1);
  psi(0)=0;
}


double Liouvillean<true>::rate(NoTime, const LazyDensityOperator& matrix, JumpNo<0>) const
{
  return aJumpRate(matrix,kappa_*(nTh_+1));
}


double Liouvillean<true>::rate(NoTime, const LazyDensityOperator& matrix, JumpNo<1>) const
{
  return 2.*kappa_*nTh_*(photonNumber(matrix)+1.);
}


template<>
void Liouvillean<false,false>::doActWithJ(NoTime, StateVectorLow& psi) const
{
  aJump(psi,kappa_);
}

template<>
void Liouvillean<false,true >::doActWithJ(NoTime, StateVectorLow& psi) const
{
  aJump(psi,kappa_);
}


template<>
double Liouvillean<false,false>::rate(NoTime, const LazyDensityOperator& matrix) const
{
  return aJumpRate(matrix,kappa_);
}


template<>
double Liouvillean<false,true>::rate(NoTime, const LazyDensityOperator&) const
{
  return -1;
}


///////////
//
// Averaged
//
///////////


namespace {

typedef cpputils::KeyPrinter::KeyLabels KeyLabels;

const KeyLabels Assemble(const KeyLabels& first, const KeyLabels& middle, const KeyLabels& last=KeyLabels())
{
  KeyLabels res(first); boost::assign::push_back(res).range(middle).range(last);
  return res;
}

}


Averaged::Averaged(const KeyLabels& follow, const KeyLabels& precede)
  : Base(keyTitle,
         Assemble(precede,KeyLabels{"<number operator>","VAR(number operator)","real(<ladder operator>)","imag(\")"},follow))
{
}


const Averaged::Averages Averaged::average_v(NoTime, const LazyDensityOperator& matrix) const
{
  Averages averages(4);

  averages=0;

  for (int n=1; n<int(matrix.getDimension()); n++) {

    double diag=matrix(n);
    averages(0)+=  n*diag;
    averages(1)+=n*n*diag;

    dcomp offdiag(sqrt(n)*matrix(n,n-1));
    averages(2)+=real(offdiag);
    averages(3)+=imag(offdiag);

  }

  return averages;

}


void Averaged::process_v(Averages& averages) const
{
  averages(1)-=sqr(averages(0));
}



AveragedQuadratures::AveragedQuadratures(const KeyLabels& follow, const KeyLabels& precede)
  : Averaged(Assemble(KeyLabels{"VAR(X)","VAR(Y)","COV(X,Y)"},follow),precede)
{
}


const AveragedQuadratures::Averages AveragedQuadratures::average_v(NoTime t, const LazyDensityOperator& matrix) const
{
  Averages averages(7);

  averages=0;

  averages(blitz::Range(0,3))=Averaged::average_v(t,matrix);

  for (int n=2; n<int(matrix.getDimension()); n++) {

    dcomp  offdiag(sqrt(n*(n-1.))*matrix(n,n-2));
    averages(4)+=real(offdiag);
    averages(5)+=imag(offdiag);

  }

  return averages;

}


void AveragedQuadratures::process_v(Averages& averages) const
{
  {
    Averages ranged(averages(blitz::Range(0,3)));
    Averaged::process_v(ranged);
  }

  double 
    Re_aSqrExpVal=averages(4),
    Re_aExpVal=averages(2),
    Im_aExpVal=averages(3);
    
  double
    a=averages(0)+.5+Re_aSqrExpVal-2*sqr(Re_aExpVal), // <X^2>-<X>^2
    b=averages(0)+.5-Re_aSqrExpVal-2*sqr(Im_aExpVal), // <Y^2>-<Y>^2
    c=averages(5)-2*Re_aExpVal*Im_aExpVal; // <(XY+YX)/2>-<X><Y>

  averages(4)=a;
  averages(5)=b;
  averages(6)=c;
  
  /*
  Eigenvalues of the real matrix
    ( a c )
    ( c b )
  */

  /*
    double
    temp1=(a+b)/2,
    temp2=sqrt(sqr(a-b)+4*sqr(c))/2;

    averages(4)=temp1+temp2;
    averages(5)=temp1-temp2;
    
    averages(6)=atan((-a+b)/(2*c)+temp2/c);
  */
}


//////////
//
// Helpers
//
//////////


const Tridiagonal aop(size_t dim)
{
  typedef Tridiagonal::Diagonal Diagonal;
  Diagonal diagonal(dim-1);
  return Tridiagonal(Diagonal(),1,Diagonal(),diagonal=sqrt(blitz::tensor::i+1.));
}



// This returns a Tridiagonal furnished with frequencies, when mode is derived from mode::Exact

const Tridiagonal aop(Ptr mode)
{
  size_t dim=mode->getDimension();
  Tridiagonal res(aop(dim));
  if (const mode::Exact* exact=dynamic_cast<const mode::Exact*>(mode.get())) res.furnishWithFreqs(mainDiagonal(exact->get_zI(),dim));
  return res;
}



const Tridiagonal nop(Ptr mode)
{
  return Tridiagonal(mainDiagonal(1.,mode->getDimension()));
}



double photonNumber(const StateVectorLow& psi)
{
  using blitz::tensor::i;
  return sum(i*blitzplusplus::sqrAbs(psi(i)));
}


double photonNumber(const LazyDensityOperator& matrix)
{
  double res=0;
  for (size_t n=1; n<matrix.getDimension(); ++n)
    res+=n*matrix(n);
  return res;
}


const StateVector coherent(const dcomp& alpha, size_t dim)
{
  StateVector res(dim,false);
  double norm(exp(-sqr(abs(alpha))/2.));

  for (size_t n=0; n<dim; ++n) res()(n)=norm*coherentElement(n,alpha);

  return res;

}


const StateVector fock(size_t n, size_t dim, double phase) throw(PrepError)
{
  if (n>=dim) throw PrepError();
  StateVector res(dim);
  res()(n)=exp(DCOMP_I*phase);
  return res;
}


const StateVector init(const Pars& p)
{
  return p.minitFock ? fock(p.minitFock,p.cutoff) : coherent(p.minit,p.cutoff);
}


} // mode




ModeBase::ModeBase(size_t dim, const RealFreqs& realFreqs, const ComplexFreqs& complexFreqs, const string& keyTitle)
    : Free(dim,realFreqs,complexFreqs)
{
  getParsStream()<<"# "<<keyTitle<<endl;
}




PumpedLossyModeIP_NoExact::PumpedLossyModeIP_NoExact(const mode::ParsPumpedLossy& p)
  : ModeBase(p.cutoff),
    structure::TridiagonalHamiltonian<1,true>(furnishWithFreqs(mode::pumping(p.eta,p.cutoff),
                                                               mode::mainDiagonal(dcomp(p.kappa,-p.delta),p.cutoff))),
    structure::ElementLiouvillean<1,1,true>(mode::keyTitle,"excitation loss"),
    structure::ElementAveraged<1,true>(mode::keyTitle,{"<number operator>","real(<ladder operator>)","imag(\")"}),
    z_(p.kappa,-p.delta)
{
  getParsStream()<<"# Interaction picture, not derived from Exact\n";
}



double PumpedLossyModeIP_NoExact::rate(OneTime, const LazyDensityOperator& m) const
{
  return mode::photonNumber(m);
}


void PumpedLossyModeIP_NoExact::doActWithJ(OneTime t, StateVectorLow& psi) const
{
  dcomp fact=sqrt(2.*real(z_))*exp(-z_*double(t));
  int ubound=psi.ubound(0);
  for (int n=0; n<ubound; ++n)
    psi(n)=fact*sqrt(n+1)*psi(n+1);
  psi(ubound)=0;
}



const PumpedLossyModeIP_NoExact::Averages PumpedLossyModeIP_NoExact::average_v(OneTime t, const LazyDensityOperator& matrix) const
{
  Averages averages(3);

  averages=0;

  for (int n=1; n<int(matrix.getDimension()); n++) {

    averages(0)+=n*exp(-2*real(z_)*n*t)*matrix(n);

    dcomp offdiag(sqrt(n)*matrix(n,n-1)*exp(-(z_+2*(n-1)*real(z_))*double(t)));
    averages(1)+=real(offdiag);
    averages(2)+=imag(offdiag);

  }

  return averages;

}