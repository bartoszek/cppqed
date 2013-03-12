// -*- C++ -*-
#ifndef   ELEMENTS_INTERACTIONS_DETAILS_BINARYINTERACTIONGENERATOR_H_INCLUDED
#define   ELEMENTS_INTERACTIONS_DETAILS_BINARYINTERACTIONGENERATOR_H_INCLUDED

#include "SmartPtr.h"

struct EmptyAveragingBaseForInteractions {};

#endif // ELEMENTS_INTERACTIONS_DETAILS_BINARYINTERACTIONGENERATOR_H_INCLUDED


#ifndef   BIG_ADDITIONAL_TEMPLATE_PARAMETERS
#define   BIG_ADDITIONAL_TEMPLATE_PARAMETERS
#endif // BIG_ADDITIONAL_TEMPLATE_PARAMETERS

#ifndef   BIG_ADDITIONAL_TEMPLATE_PARAMETERS_PASS
#define   BIG_ADDITIONAL_TEMPLATE_PARAMETERS_PASS
#endif // BIG_ADDITIONAL_TEMPLATE_PARAMETERS_PASS

#ifndef   BIG_ADDITIONAL_PARAMETERS
#define   BIG_ADDITIONAL_PARAMETERS
#endif // BIG_ADDITIONAL_PARAMETERS

#ifndef   BIG_ADDITIONAL_PARAMETERS_PASS
#define   BIG_ADDITIONAL_PARAMETERS_PASS
#endif // BIG_ADDITIONAL_PARAMETERS_PASS


#define BASE_class BIG_NAMESPACE_NAME::Base BIG_ADDITIONAL_TEMPLATE_PARAMETERS_PASS


template<BIG_ADDITIONAL_TEMPLATE_PARAMETERS typename A=EmptyAveragingBaseForInteractions>
class BIG_CLASS_NAME : public BASE_class, public A
{
public:

  template<typename F1, typename F2>
  BIG_CLASS_NAME(const F1& f1, const F2& f2 BIG_ADDITIONAL_PARAMETERS)
    : BASE_class(cpputils::sharedPointerize(f1),cpputils::sharedPointerize(f2) BIG_ADDITIONAL_PARAMETERS_PASS),
      A()
  {}

};


#undef BASE_class

#undef BIG_ADDITIONAL_TEMPLATE_PARAMETERS_PASS
#undef BIG_ADDITIONAL_TEMPLATE_PARAMETERS
#undef BIG_ADDITIONAL_PARAMETERS_PASS
#undef BIG_ADDITIONAL_PARAMETERS
#undef BIG_NAMESPACE_NAME
#undef BIG_CLASS_NAME