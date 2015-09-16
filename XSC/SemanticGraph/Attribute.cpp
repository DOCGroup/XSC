// file      : XSC/SemanticGraph/Attribute.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include <XSC/SemanticGraph/Attribute.hpp>

namespace XSC
{
  namespace SemanticGraph
  {
    using Introspection::TypeInfo;
    using Introspection::Access;

    // Attribute
    //
    //
    namespace
    {
      TypeInfo
      attribute_init_ ()
      {
        TypeInfo ti (typeid (Attribute));
        ti.add_base (Access::PUBLIC, true, Instance::static_type_info ());
        return ti;
      }

      TypeInfo attribute_ (attribute_init_ ());
    }

    TypeInfo const& Attribute::
    static_type_info () { return attribute_; }
  }
}
