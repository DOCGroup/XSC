// file      : XSC/SemanticGraph/Complex.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>

#include <XSC/SemanticGraph/Complex.hpp>

namespace XSC
{
  namespace SemanticGraph
  {
    using Introspection::TypeInfo;
    using Introspection::Access;
   
    namespace
    {
      TypeInfo
        complex_init_ ()
      {
        TypeInfo ti (typeid (Complex));
        ti.add_base (Access::PUBLIC, true, Type::static_type_info ());
        ti.add_base (Access::PUBLIC, true, Scope::static_type_info ());
        return ti;
      }
     
      TypeInfo complex_ (complex_init_ ());
    }
   
    TypeInfo const& Complex::
    static_type_info () { return complex_; }
  }
}
