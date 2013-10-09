#include "ComplexArrayExtensions.tcc"
#include "VectorFromMatrixSliceIterator.tcc"

#include "Algorithm.h"
#include "MathExtensions.h"
#include "Randomized.h"
#include "Range.h"

#define BOOST_TEST_MODULE BlitzExtensions test
#include <boost/test/unit_test.hpp>

#include <boost/bind.hpp>


using namespace std;
using namespace randomized;
using namespace blitzplusplus;
using namespace linalg;
using namespace mathutils;

using cpputils::for_each;

const double epsilonCmp=1e-12;

void myFunc(const CArray<6>& aa, const CArray<3>& vv, CArray<3>& ww)
{
  using namespace blitz::tensor;
  CArray<6> temp(aa.shape());
  temp=aa(i,j,k,l,m,n)*vv(l,m,n);
  ww=sum(sum(sum(temp,n),m),l);
}


const int RA=4;

typedef CArray<  RA> CAR ;
typedef CArray<2*RA> CA2R;

typedef CArray<RA-1> CARM1;
typedef CArray<RA+1> CARP1;


Randomized::Ptr ran(MakerGSL()(1001));



BOOST_AUTO_TEST_CASE( TwoTimesRealPartOfSelfTest )
{ 
  ExtTiny<RA/2> dims(6,4);
  CAR array(concatenateTinies(dims,dims));
  fillWithRandom(array,ran);

  CAR arrayHC(hermitianConjugate(array));
  CAR arrayReal(array.shape()); arrayReal=(array+arrayHC);

  CAR arrayLow(array.copy());
  CMatrix matrixView(binaryArray(arrayLow));
  calculateTwoTimesRealPartOfSelf(matrixView);

  //cout<<array;
  //cout<<array.ordering()<<' '<<array.stride()<<' '<<array.shape()<<endl;
  //cout<<arrayHC.ordering()<<' '<<arrayHC.stride()<<' '<<arrayHC.shape()<<endl;
  //cout<<matrixView.ordering()<<' '<<matrixView.stride()<<' '<<matrixView.shape()<<endl;

  hermitianConjugateSelf(array);

  BOOST_CHECK(all(arrayReal==hermitianConjugate(arrayReal)));
  cout<<"Hermitian conjugation OK\n";
  BOOST_CHECK(all(array==arrayHC));
  cout<<"In-place Hermitian conjugation OK\n";
  BOOST_CHECK(all(arrayLow ==hermitianConjugate(arrayLow )));
  BOOST_CHECK(all(arrayLow ==arrayReal                    ));
  cout<<"In-place real part of matrices OK\n";
    
}



BOOST_AUTO_TEST_CASE( DoDirectTest )
{
  ExtTiny<RA-1> dims0(4,5,2);
  ExtTiny<RA+1> dims1(3,4,2,4,5);

  CARM1 am1(dims0);
  CARP1 ap1(dims1);

  fillWithRandom(ap1,fillWithRandom(am1,ran));

  CA2R a2r(concatenateTinies(dims0,dims1));
  {
    using namespace blitz::tensor;
    a2r=am1(i,j,k)*ap1(l,m,n,o,p);
  }

  BOOST_CHECK(all(doDirect<dodirect::multiplication>(am1,ap1)==a2r));
  cout<<"Direct product OK\n";

  {
    using namespace blitz::tensor;
    a2r=am1(i,j,k)+ap1(l,m,n,o,p);
  }

  BOOST_CHECK(all(doDirect<dodirect::addition>(am1,ap1)==a2r));
  cout<<"Direct sum OK\n";
}



BOOST_AUTO_TEST_CASE( MatrixWithVectorMultiplication ) // computing v*a (v acting on only certain indices) in two ways: with tensor and basi
{ 
  ExtTiny<5> dims1(3,4,2,6,5);
  ExtTiny<3> dims0(dims1(2),dims1(1),dims1(4));

  CArray<6> a(concatenateTinies(dims0,dims0));
  CArray<5> v(dims1), vResTensor(dims1), vResBASI(dims1);

  fillWithRandom(v,fillWithRandom(a,ran));

  {
    using namespace blitz::tensor;
    CArray<8> temp8(concatenateTinies(
					  concatenateTinies(
							    dims0,
							    ExtTiny<2>(dims1(0),dims1(3))
							    ),
					  dims0
					  ));
    temp8=a(i,j,k,n,o,p)*v(l,o,n,m,p);
    vResTensor=CArray<5>(sum(sum(sum(temp8,p),o),n)).transpose(3,1,0,4,2);
  }
    
  {
    typedef tmptools::Vector<2,1,4> V214;
    for_each(blitzplusplus::basi::fullRange<V214>(v),blitzplusplus::basi::begin<V214>(vResBASI),bind(myFunc,a,_1,_2));
  }

  BOOST_CHECK(all(vResBASI==vResTensor));
  cout<<"BlitzArraySliceIterator OK\n";
}



BOOST_AUTO_TEST_CASE( MatrixProducts ) // VFMSI for matrix products a*rho
{ 
  ExtTiny<3> dims0(4,5,2);
  CArray<6> rho(concatenateTinies(dims0,dims0)), a(rho.shape()), resTensor(rho.shape());

  fillWithRandom(rho,fillWithRandom(a,ran));

  {
    using namespace blitz::tensor;
    CArray<9> temp9(concatenateTinies(rho.shape(),dims0));
    temp9=a(i,j,k,o,p,q)*rho(o,p,q,l,m,n);
    resTensor=CArray<6>(sum(sum(sum(temp9,q),p),o));
  }

  {
    using namespace blitzplusplus::vfmsi;
    for_each(fullRange<Left>(rho),bind(myFunc,a,_1,_1));
  }
  //cout<<max(abs(rho-resTensor))<<endl;
  //BOOST_CHECK(!fcmp(1-max(abs(rho-hermitianConjugate(rho))),1,epsilonCmp));
  BOOST_CHECK(all(rho==resTensor));
  cout<<"VectorFromMatrixSliceIterator matrix product OK\n";
}



BOOST_AUTO_TEST_CASE( VFMSI_Test ) // VFMSI computing a*rho*adagger (rho Hermitian)
{ 
  ExtTiny<3> dims0(4,5,2);
  CArray<6> rho(concatenateTinies(dims0,dims0)), a(rho.shape()), resTensor(rho.shape());
  CMatrix matrixView(binaryArray(rho));

  fillWithRandom(rho,fillWithRandom(a,ran));

  calculateTwoTimesRealPartOfSelf(matrixView);
  {
    using namespace blitz::tensor;
    CArray<12> temp12(concatenateTinies(rho.shape(),rho.shape()));
    temp12=a(i,j,k,o,p,q)*rho(o,p,q,r,s,t)*conj(a(l,m,n,r,s,t));
    resTensor=CArray<6>(sum(sum(sum(sum(sum(sum(temp12,t),s),r),q),p),o));
  }
  BOOST_CHECK(!fcmp(1-max(abs(resTensor-hermitianConjugate(resTensor))),1,epsilonCmp));
  {
    {
      using namespace blitzplusplus::vfmsi;
      for_each(fullRange<Left>(rho),bind(myFunc,a,_1,_1));
    }
    hermitianConjugateSelf(rho);
    {
      using namespace blitzplusplus::vfmsi;
      for_each(fullRange<Left>(rho),bind(myFunc,a,_1,_1));
    }
  }
  //cout<<max(abs(rho-resTensor))<<endl;
  BOOST_CHECK(!fcmp(1-max(abs(rho-hermitianConjugate(rho))),1,epsilonCmp));
  BOOST_CHECK(!fcmp(1-max(abs(rho-resTensor)),1,epsilonCmp));
  cout<<"VectorFromMatrixSliceIterator matrix sandwich OK\n";
}
