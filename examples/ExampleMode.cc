#include "ExampleMode.h"

#include <boost/assign/list_of.hpp>

#include <boost/bind.hpp>

using boost::assign::list_of; using boost::bind;


void aJump   (StateVectorLow&, double kappa_nPlus1);
void aDagJump(StateVectorLow&, double kappa_n     );

double aJumpRate   (const LazyDensityOperator&, double kappa_nPlus1);
double aDagJumpRate(const LazyDensityOperator&, double kappa_n     );


PumpedLossyMode::PumpedLossyMode(double delta, double kappa, dcomp eta, double n, size_t cutoff)
  : Free(cutoff,
         RealFreqs(),
         FREQS("(kappa*(2*n+1),delta)",dcomp(kappa*(2*n+1),delta),cutoff      )
              ("eta"                  ,eta                       ,sqrt(cutoff))),
    TridiagonalHamiltonian<1,false>(dcomp(-kappa*(2*n+1),delta)*nop(cutoff)
                                    +
                                    tridiagPlusHC_overI(conj(eta)*aop(cutoff))),
    ElementLiouvillean<1,2>(JumpStrategies(bind(aJump   ,_1,kappa*(n+1)),
                                           bind(aDagJump,_1,kappa* n   )),
                            JumpRateStrategies(bind(aJumpRate   ,_1,kappa*(n+1)),
                                               bind(aDagJumpRate,_1,kappa* n   )),
                            "Mode",list_of("photon loss")("photon absorption")),
    ElementAveraged<1>("PumpedLossyMode",
                       list_of("<number operator>")("real(<ladder operator>)")("imag(\")"))
{
  getParsStream()<<"# Pumped lossy mode";
}


const PumpedLossyMode::Averages PumpedLossyMode::average_v(const LazyDensityOperator& matrix) const
{
  Averages averages(3);

  averages=0;

  averages(0)=aJumpRate(matrix,1);

  for (int n=1; n<int(matrix.getDimension()); n++) {
    double sqrtn=sqrt(n);
    dcomp offdiag(matrix(n,n-1));
    averages(1)+=sqrtn*real(offdiag);
    averages(2)+=sqrtn*imag(offdiag);
  }

  return averages;

}


const Tridiagonal::Diagonal mainDiagonal(const dcomp& z, size_t cutoff);


PumpedLossyModeIP::PumpedLossyModeIP(double delta, double kappa, dcomp eta, double n, size_t cutoff)
  : Free(cutoff,
         FREQS("kappa*(2*n+1)",kappa*(2*n+1),cutoff)
              ("delta",delta,1),
         FREQS("eta",eta,sqrt(cutoff))),
    FreeExact(cutoff),
    TridiagonalHamiltonian<1,true>(furnishWithFreqs(tridiagPlusHC_overI(conj(eta)*aop(cutoff)),
                                                    mainDiagonal(dcomp(kappa*(2*n+1),-delta),cutoff))),
    ElementLiouvillean<1,2>(JumpStrategies(bind(aJump   ,_1,kappa*(n+1)),
                                           bind(aDagJump,_1,kappa* n   )),
                            JumpRateStrategies(bind(aJumpRate   ,_1,kappa*(n+1)),
                                               bind(aDagJumpRate,_1,kappa* n   )),
                            "Mode",list_of("photon loss")("photon absorption")),
    ElementAveraged<1>("PumpedLossyMode",
                       list_of("<number operator>")("real(<ladder operator>)")("imag(\")")),
    z_(kappa*(2*n+1),-delta)
{}


void PumpedLossyModeIP::updateU(double dtDid) const
{
  getDiagonal()=exp(-z_*(dtDid*blitz::tensor::i));
}


const PumpedLossyModeIP::Averages PumpedLossyModeIP::average_v(const LazyDensityOperator& matrix) const
{
  Averages averages(3);

  averages=0;

  averages(0)=aJumpRate(matrix,1);

  for (int n=1; n<int(matrix.getDimension()); n++) {
    double sqrtn=sqrt(n);
    dcomp offdiag(matrix(n,n-1));
    averages(1)+=sqrtn*real(offdiag);
    averages(2)+=sqrtn*imag(offdiag);
  }

  return averages;

}
