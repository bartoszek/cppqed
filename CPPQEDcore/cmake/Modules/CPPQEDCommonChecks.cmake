if(Boost_FOUND)
  if(Boost_MAJOR_VERSION EQUAL "1" AND Boost_MINOR_VERSION GREATER "51")
    add_definitions(-DBOOST_RESULT_OF_USE_TR1)
    message(STATUS "added -DBOOST_RESULT_OF_USE_TR1" )
  endif()
endif()

if(CBLAS_FOUND AND "${CBLAS_LIBRARIES}" MATCHES "gslcblas")
  add_definitions(-DGSL_CBLAS)
  message(STATUS "added -DGSL_CBLAS" )
endif(CBLAS_FOUND AND "${CBLAS_LIBRARIES}" MATCHES "gslcblas")


MACRO(CPPQED_CXX_FLAGS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-sign-compare")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DBZ_DEBUG -Wall -Wextra -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -Wno-deprecated -Wnon-virtual-dtor -Wno-ignored-qualifiers -Wno-sign-compare")
  if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++11-extensions  -Wno-local-type-template-args")
  endif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
ENDMACRO(CPPQED_CXX_FLAGS)