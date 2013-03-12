// -*- C++ -*-

template<typename V_S, typename A>
inline
const RETURN_type1(true )
begin(const A& array ADDITIONAL_ARGUMENT)
{return RETURN_type1(true )(array ADDITIONAL_PARAMETER, boost::mpl::false_());}

template<typename V_S, typename A>
inline
const RETURN_type1(true )
end  (const A& array ADDITIONAL_ARGUMENT)
{return RETURN_type1(true )(array ADDITIONAL_PARAMETER, boost::mpl:: true_());}

template<typename V_S, typename A>
inline
const RETURN_type1(false)
begin(      A& array ADDITIONAL_ARGUMENT)
{return RETURN_type1(false)(array ADDITIONAL_PARAMETER, boost::mpl::false_());}

template<typename V_S, typename A>
inline
const RETURN_type1(false)
end  (      A& array ADDITIONAL_ARGUMENT)
{return RETURN_type1(false)(array ADDITIONAL_PARAMETER, boost::mpl:: true_());}


#define RETURN_type2(CONST) boost::iterator_range<RETURN_type1(CONST)>

template<typename V_S, typename A>
inline
const RETURN_type2(true ) 
fullRange(const A& array ADDITIONAL_ARGUMENT)
{return RETURN_type2(true )(blitzplusplus::NS_NAME::begin<V_S,A>(array ADDITIONAL_PARAMETER),blitzplusplus::NS_NAME::end<V_S,A>(array ADDITIONAL_PARAMETER));}

template<typename V_S, typename A>
inline
const RETURN_type2(false) 
fullRange(      A& array ADDITIONAL_ARGUMENT)
{return RETURN_type2(false)(blitzplusplus::NS_NAME::begin<V_S,A>(array ADDITIONAL_PARAMETER),blitzplusplus::NS_NAME::end<V_S,A>(array ADDITIONAL_PARAMETER));}


#undef  RETURN_type2
#undef  ADDITIONAL_ARGUMENT
#undef  ADDITIONAL_PARAMETER
#undef  RETURN_type1
#undef  NS_NAME