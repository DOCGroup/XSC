// file      : XSC/SemanticGraph/Enumeration.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : $Id$

#include <XSC/SemanticGraph/Enumeration.hpp>

namespace XSC
{
  namespace SemanticGraph
  {
    using Introspection::TypeInfo;
    using Introspection::Access;

    // Enum
    //
    //
    namespace
    {
      TypeInfo
      enumeration_init_ ()
      {
        TypeInfo ti (typeid (Enumeration));
        ti.add_base (Access::PUBLIC, true, Complex::static_type_info ());
        return ti;
      }

      TypeInfo enumeration_ (enumeration_init_ ());
    }

    TypeInfo const& Enumeration::
    static_type_info () { return enumeration_; }


    // Enumerator
    //
    //
    namespace
    {
      TypeInfo
      enumerator_init_ ()
      {
        TypeInfo ti (typeid (Enumerator));
        ti.add_base (Access::PUBLIC, true, Instance::static_type_info ());
        return ti;
      }

      TypeInfo enumerator_ (enumerator_init_ ());
    }

    TypeInfo const& Enumerator::
    static_type_info () { return enumerator_; }
  }
}
