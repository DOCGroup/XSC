// file      : XSC/SemanticGraph/Element.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include <XSC/SemanticGraph/Element.hpp>

namespace XSC
{
  namespace SemanticGraph
  {
    using Introspection::TypeInfo;
    using Introspection::Access;

    // Element
    //
    //
    namespace
    {
      TypeInfo
      element_init_ ()
      {
        TypeInfo ti (typeid (Element));
        ti.add_base (Access::PUBLIC, true, Instance::static_type_info ());
        return ti;
      }

      TypeInfo element_ (element_init_ ());
    }

    TypeInfo const& Element::
    static_type_info () { return element_; }
  }
}
