// -*- C++ -*-

#include "PythonExtension.h"
#include "Namespaces.h"

#include "Interaction.h"

using namespace boost::python;

using structure::Interaction;

namespace pythonext {

void export_Interaction() {
  scope namespaceScope = structureNameSpace;
  class_<Interaction<2>, boost::noncopyable >("Interaction2", no_init);
  class_<Interaction<3>, boost::noncopyable >("Interaction3", no_init);
  register_ptr_to_python<Interaction<2>::Ptr>();
  implicitly_convertible<boost::shared_ptr<Interaction<2>>, Interaction<2>::Ptr>();
  register_ptr_to_python<Interaction<3>::Ptr>();
  implicitly_convertible<boost::shared_ptr<Interaction<3>>, Interaction<3>::Ptr>();
}


} // pythonext