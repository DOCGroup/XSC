// file      : XSC/SemanticGraph/Namespace.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include <XSC/SemanticGraph/Namespace.hpp>

namespace XSC
{
  namespace SemanticGraph
  {
    using Introspection::TypeInfo;
    using Introspection::Access;

    namespace
    {
      TypeInfo
      namespace_init_ ()
      {
        TypeInfo ti (typeid (Namespace));
        ti.add_base (Access::PUBLIC, true, Scope::static_type_info ());
        return ti;
      }

      TypeInfo namespace_ (namespace_init_ ());
    }

    TypeInfo const& Namespace::
    static_type_info () { return namespace_; }
  }
}
