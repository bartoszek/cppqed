// -*- C++ -*-
#ifndef   CPPQEDCORE_QUANTUMTRAJECTORY_MASTERFWD_H_INCLUDED
#define   CPPQEDCORE_QUANTUMTRAJECTORY_MASTERFWD_H_INCLUDED

#include "TMP_Tools.h"


namespace quantumtrajectory {

namespace master {

struct SystemNotApplicable;


template<int RANK>
class Base;

} // master


template<int RANK, typename V=tmptools::V_Empty, bool IS_FAST=false>
class Master;

} // quantumtrajectory


#endif // CPPQEDCORE_QUANTUMTRAJECTORY_MASTERFWD_H_INCLUDED
