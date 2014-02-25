// -*- C++ -*-
#include "PythonExtension.h"
#include "Namespaces.h"

#include "FormDouble.h"
#include "Pars.h"
#include "ParsStochasticTrajectory.h"

#include "ParsPropertyMacros.h"

using namespace boost::python;

using trajectory::ParsStochastic;
using trajectory::ParsEvolved;
using std::string;

namespace pythonext {

PARS_GETTER_SETTER(unsigned long, ParsStochastic, seed)
PARS_GETTER_SETTER(bool, ParsStochastic, noise)
PARS_GETTER_SETTER(size_t, ParsStochastic, nTraj)


void export_ParsTrajectory_ParsStochasticTrajectory()
{
  scope namespaceScope = trajectoryNameSpace;
  class_<ParsStochastic, bases<ParsEvolved> >
    (
      "ParsStochastic",
      init<parameters::ParameterTable&, optional<const std::string&> >()
        [with_custodian_and_ward<1,2>()]
    )
    .PARS_PROPERTY(seed)
    .PARS_PROPERTY(noise)
    .PARS_PROPERTY(nTraj)
  ;
}

} // pythonext